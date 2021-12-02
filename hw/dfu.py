# -*- coding: utf-8 -*-

import os, os.path, shutil, subprocess

def merge_image(binaries, build_dir="build", build_name="top"):
    image_path = f"{build_dir}/{build_name}.bin"
    image      = open(image_path, "wb")

    for bin_name in binaries:
        bin_path = binaries[bin_name][0].format(build_dir=build_dir, build_name=build_name)
        if not os.path.isfile(bin_path):
            print("merge_image: {} does not exist, skipping".format(bin_name))
            continue
        print("merge_image: Adding {} up to {} bytes".format(bin_name, binaries[bin_name][1]))
        if os.stat(bin_path).st_size > 0:
            bin_file = open(bin_path, "rb")
            for i in range(0x0, binaries[bin_name][1]):
                b = bin_file.read(1)
                if not b:
                    image.write(0xff.to_bytes(1, "big"))
                else:
                    image.write(b)
            bin_file.close()

    print("merge_image: {} written".format(image_path))
    image.close()

def add_dfu_suffix(fn, vid='5bf0', pid='1209'):
    fn_base, _ext = os.path.splitext(fn)
    fn_dfu = fn_base + '.dfu'
    shutil.copyfile(fn, fn_dfu)
    subprocess.check_call(['dfu-suffix', '--pid', pid, '--vid', vid, '--add', fn_dfu])
