import os

def print_dirs(path):
    for root, dirs, files in os.walk(path):
        # Remove directories that contain .git or .venv from the traversal
        dirs[:] = [d for d in dirs if ".git" not in d and ".venv" not in d and ".idea" not in d]

        # Print files once per directory level
        for f in files:
            print(os.path.join(root, f).removeprefix(path+"/"))

if __name__ == '__main__':
    print_dirs('../WaffleOS/source')