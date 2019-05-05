from .data import generalData


def sort(a):
    _len = len(a)
    quick_sort(a, 0, _len - 1)


def quick_sort(a, p, r):
    if p >= r:
        return
    q = partition2(a, p, r)
    quick_sort(a, p, q)
    quick_sort(a, q+1, r)


def partition2(arr, l, r):
    pivot = arr[l]
    index = l+1
    print(arr[l:r+1])
    for i in range(l+1, r+1):
        if arr[i] < pivot:
            if i != index:
                arr[i], arr[index] = arr[index], arr[i]
                print(arr[l:r+1])            
            index += 1

    index -= 1
    if index > l:
        arr[l], arr[index] = arr[index], arr[l]
    return index


def partition3(a, start, end):
    pivot = a[start]
    index = start + 1

    for i in range(index, end+1):
        if a[i] < pivot:
            if i != pivot:
                a[index], a[i] = a[i], a[index]
            index += 1
    index -= 1
    a[start], a[index] = a[index], a[start]
    return index


if __name__ == "__main__":
    data = generalData(20)
    sort(data)
    print(data)
