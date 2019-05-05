from .data import generalData


def merge_sort(li):
    if len(li) == 1:
        return li
    mid = len(li) >> 1
    left = li[:mid]
    right = li[mid:]
    ll = merge_sort(left)
    rl = merge_sort(right)
    return merge(ll, rl)


def merge(left, right):
    result = []
    while len(left) > 0 and len(right) > 0:
        if left[0] <= right[0]:
            result.append(left.pop(0))
        else:
            result.append(right.pop(0))
    result += left
    result += right
    return result


if __name__ == "__main__":
    data = generalData(20)
    print(data)
    res = merge_sort(data)
    print(res)
