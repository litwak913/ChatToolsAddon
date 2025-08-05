#!/usr/bin/env python3
import sys
import os
import hashlib

def compute_md5(filepath, chunk_size=8192):
    md5 = hashlib.md5()
    with open(filepath, 'rb') as f:
        for chunk in iter(lambda: f.read(chunk_size), b''):
            md5.update(chunk)
    return md5.hexdigest()

def main():
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <dir>")
        sys.exit(1)

    target_dir = sys.argv[1]
    summary_path = os.environ.get('GITHUB_STEP_SUMMARY')
    if not summary_path:
        print("No GITHUB_STEP_SUMMARY!")
        sys.exit(1)

    entries = sorted(os.listdir(target_dir))
    with open(summary_path, 'a', encoding='utf-8') as summary:
        summary.write("## MD5 Checksums`\n\n")
        for name in entries:
            full = os.path.join(target_dir, name)
            if os.path.isfile(full):
                checksum = compute_md5(full)
                md5_file = full + '.md5'
                with open(md5_file, 'w', encoding='utf-8') as mf:
                    mf.write(f"{checksum}")
                summary.write(f"- `{name}`: `{checksum}`\n")

if __name__ == '__main__':
    main()
