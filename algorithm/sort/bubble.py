from .data import generalData


def sort(a):
    _len = len(a)
    for i in range(_len):
        switch = False
        for j in range(_len, -i, -1):
            if a[j+1] > a[j]:
                a[j], a[j+1] = a[j+1], a[j]
                switch = True
        if not switch:
            break


if __name__ == "__main__":
    data = generalData(10000)
    sort(data)
