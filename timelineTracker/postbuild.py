import sys
import os
import subprocess
import collections
import shutil

# Helper type for dependencies copy.
CopyInfo = collections.namedtuple("CopyInfo", "srcPath, dstPath, name")

# Copy file or folder.
def copy(copyInfo):
    if not os.path.exists(copyInfo.dstPath):
        os.makedirs(copyInfo.dstPath)
    srcItem = os.path.join(copyInfo.srcPath, copyInfo.name)
    dstItem = os.path.join(copyInfo.dstPath, copyInfo.name)
    if os.path.isfile(srcItem):
        shutil.copy2(srcItem, dstItem)

# Script start.
print("Post-build actions starting...")

# Check arguments.
if len(sys.argv) < 5:
    print("Invalid input arguments, aborting...")
    print("Usage: python " + __file__ + " <path_to_code_root_dir> <path_to_output_directory> <target_executable_name> <path_to_source_qml_files>")
    sys.exit(-1)

rootDir = sys.argv[1]
outDir = sys.argv[2]
exeName = sys.argv[3]
qmlRootDir = sys.argv[4]

# Run standard Qt deploy app.
deployCommand = ["windeployqt", "--compiler-runtime", "--qmldir", qmlRootDir, os.path.join(outDir, exeName)]
print(subprocess.check_output(deployCommand).decode('ascii'))

# Copy non-Qt dependencies.
opensslPath = os.path.join(rootDir, "dependencies/openssl-1.0.2l-i386-win32")
extraDependencies = [
    CopyInfo(srcPath = opensslPath, dstPath = outDir, name = "ssleay32.dll"),
    CopyInfo(srcPath = opensslPath, dstPath = outDir, name = "libeay32.dll")
]
for dependency in extraDependencies:
    copy(dependency)

print("Post-build completed")