from .data import generalData


def sort(a):
    _len = len(a)
    
    for i in range(1, _len):
        j = 0
        while j < i:
            if a[i] > a[j]:
                a[i], a[j] = a[j], a[i]
                print(a)
            j += 1


if __name__ == "__main__":
    data = generalData(20)
    sort(data)
    print(data)
