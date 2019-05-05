from .data import generalData


def sort(a):
    _len = len(a)
    for i in range(_len):
        for j in range(i, _len): 
            if a[j] < a[i]:
                a[i], a[j] = a[j], a[i]
                print(a)


if __name__ == "__main__":
    data = generalData(20)
    sort(data)
