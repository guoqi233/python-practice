"""
从一段序列中反推出，帧号的字符串表示
比如(1,2,3,4,5,6,7,8,9),返回1-9；
(1,2,3,4,6,7,8,9),返回1-4，6-9。
"""

import random
SIZE = 100


def format_frames_range(arr):
    length = len(arr)
    res = list()
    frames_range = list()
    res.append(1)
    cur_len = 1
    for i in range(1, length):
        if (arr[i] - arr[i-1]) == 1:
            cur_len += 1
        else:
            cur_len = 1
        res.append(cur_len)
    res.append(1)
    for i in range(0, length):
        if res[i] >= res[i + 1]:
            start = arr[i-res[i] + 1]
            end = arr[i]
            if start == end:
                item = "{0}".format(start)
            else:
                item = "{0}-{1}".format(start, end)
            frames_range.append(item)
    return ",".join(frames_range)


def main():
    a = 5
    frames = [i for i in range(SIZE)]
    while a > 0:
        try:
            frames.remove(random.randint(0, SIZE))
        except ValueError:
            pass
        finally:
            a -= 1
    print(frames)
    print(format_frames_range(frames))


if __name__ == "__main__":
    main()
