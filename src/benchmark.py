import json

def main():
    input = open("benchmark.json").read();
    retval = 0
    for i in xrange(0, 10000):
        data = json.loads(input);
        friends = data[0]["friends"]
        for friend in friends:
            if friend["id"] == 1:
                retval += 1
                break
    print retval

main()
