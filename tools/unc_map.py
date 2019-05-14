"""
windows only
"""
import re
import os
import _winreg
import subprocess
import traceback
from collections import defaultdict


class Hosts(object):
    host_file = r"C:\Windows\System32\drivers\etc\hosts"

    def __init__(self):
        self.hosts = defaultdict(set)
        self.extra_info = list()

        with open(self.host_file, "r") as fp:
            for line in fp.readlines():
                line = line.strip()
                if not line:
                    continue
                if line.startswith("#"):
                    self.extra_info.append(line)
                    continue
                else:
                    ip_address, domain_name = line.split()
                    self.hosts[ip_address].add(domain_name)

    def add_host(self, ipaddr, domain):
        self.hosts[ipaddr].add(domain)

    def write_to_hosts(self):
        with open(self.host_file, "w") as fp:
            for line in self.extra_info:
                fp.write("{0}\n".format(line))
            fp.write("\n")
            for ip_addr, domains in self.hosts.items():
                for domain in domains:
                    fp.write("{0}\t{1}\n".format(ip_addr, domain))


class Share(object):
    def __init__(self, share_name, share_path, server_path):
        self.share_name = share_name
        self.share_path = share_path
        self.server_path = server_path

    def make_link(self, share_root):
        local_path = os.path.join(share_root, self.share_name, self.share_path)
        if os.path.exists(local_path):
            os.rmdir(local_path)
        cmds = "mklink /d \"{0}\" \"{1}\"".format(local_path, self.server_path)
        print(cmds)
        p = subprocess.Popen(cmds, shell=True)
        p.wait()


class UncResource(object):
    share_root = r"c:\unc"
    unc_prefix_regex = re.compile(
        r"^\\+(?P<domain>[0-9a-z_\-.]+)\\+(?P<share_name>[0-9a-z_\-.]+)\\+(?P<share_path>.+)$",
        re.IGNORECASE
    )

    def __init__(self):
        self.domains = set()
        self.hosts = Hosts()
        self.shares = defaultdict(list)
        if not os.path.isdir(self.share_root):
            os.makedirs(self.share_root)

    def add_local_domain(self, domain):
        self.hosts.add_host("127.0.0.1", domain)
        self.domains.add(domain)

    def _set_domain_reg(self):
        """
        add all domain to windows reg
        :return:
        """
        hkey_local_machine = _winreg.OpenKey(_winreg.HKEY_LOCAL_MACHINE, r'')
        aim_key = _winreg.CreateKey(hkey_local_machine, r"SYSTEM\CurrentControlSet\Control\Lsa\MSV1_0")
        _winreg.SetValueEx(aim_key, "BackConnectionHostNames", 0, _winreg.REG_MULTI_SZ, list(self.domains))

    @staticmethod
    def del_net_share(share_name):
        try:
            subprocess.check_output("net share {0} /delete".format(share_name), stderr=subprocess.PIPE)
        except subprocess.CalledProcessError:
            pass

    @staticmethod
    def format_server_path(server_path):
        temp_path = server_path.replace("/", "\\")
        string = temp_path.replace('\\', "/")
        string = re.sub('/+', "/", string)
        res = re.sub('^/+', "//", string)
        temp_path = res.replace('/', "\\")
        return temp_path

    def unc_match(self, unc_path):
        return self.unc_prefix_regex.match(unc_path)

    def make_net_shares(self):
        self._set_domain_reg()
        for share_name in self.shares.keys():
            local_share_path = os.path.join(self.share_root, share_name)
            if not os.path.isdir(local_share_path):
                os.makedirs(local_share_path)
            try:
                subprocess.check_output("net share {0}=\"{1}\"".format(share_name, local_share_path))
            except subprocess.CalledProcessError as error:
                print(error.message)
                traceback.print_exc()

    def make_links(self):
        for share_name, shares in self.shares.items():
            for share in shares:
                share.make_link(self.share_root)

    def add_unc(self, domain, share_name, share_path, server_path):
        server_path = self.format_server_path(server_path)
        self.add_local_domain(domain)
        self.shares[share_name].append(Share(share_name=share_name, share_path=share_path, server_path=server_path))
        self.del_net_share(share_name)

    def make(self):
        self.hosts.write_to_hosts()
        self.make_net_shares()
        self.make_links()


def main():
    pathhints = [
        {
            "orig_path": r"\\origin.domain.com\storage\show",
            "server_path": r"\\example.domain.com\bee\data_store\maya_input\500\874\project\2084\unc_storage\show"
        },
    ]

    obj = UncResource()

    for pathhint in pathhints:
        match = obj.unc_match(pathhint["orig_path"])
        if match:
            result = match.groupdict()
            obj.add_unc(
                domain=result["domain"],
                share_path=result["share_path"],
                share_name=result["share_name"],
                server_path=pathhint["server_path"]
            )
    obj.make()


if __name__ == '__main__':
    main()
