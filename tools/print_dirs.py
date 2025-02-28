import os

def print_dirs(path):
    for root, dirs, files in os.walk(path):
        for d in dirs:
            print(os.path.join(root, d))
            for f in files:
                print(os.path.join(root, f))

if __name__ == '__main__':
    print_dirs('source')