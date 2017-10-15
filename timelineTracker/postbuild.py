import sys
import os
import subprocess

print("Post-build actions starting...")

if len(sys.argv) < 4:
    print("Invalid input arguments, aborting...")
    print("Usage: python " + __file__ + " <path_to_output_directory> <target_executable_name> <path_to_source_qml_files>")
    sys.exit(-1)

exePath = os.path.join(sys.argv[1], sys.argv[2])
deployCommand = ["windeployqt", "--compiler-runtime", "--qmldir", sys.argv[3], exePath]
print(subprocess.check_output(deployCommand).decode('ascii'))

print("Post-build completed")