import sys
import os
import subprocess

print("Build info script starting...")

if len(sys.argv) < 3:
    print("Invalid input arguments, aborting...")
    print("Usage: python " + __file__ + " <path_to_code_root_directory> <path_to_buildinfo_file>")
    sys.exit(-1)

os.chdir(sys.argv[1])

buildNum = subprocess.check_output(["git", "rev-list", "--count", "HEAD"]).decode('ascii').strip()
commitHash = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).decode('ascii').strip()
modifications = subprocess.check_output(["git", "status", "--porcelain"]).decode('ascii').strip()

file = open(sys.argv[2], mode='w')
file.write('#pragma once\n\n')
file.write('#define VERSION_BUILD_NUMBER (' + buildNum + ')\n')
file.write('#define VERSION_COMMIT_HASH ("' + commitHash + '")\n')
file.write('#define VERSION_HAS_MODIFICATIONS (' + str(len(modifications) > 0).lower() + ')\n')
file.close()

print("Build info created")