#!/bin/bash
# guest 작업 디렉토리 준비 (world-writable/sticky 아님, 소유자 guest)
# → fs.protected_symlinks 전역 설정과 무관하게 symlink 를 따라감(§3 설계).
mkdir -p /home/guest/work
chown guest:guest /home/guest/work
chmod 755 /home/guest/work
rm -f /home/guest/work/XYZ /home/guest/work/ZZZ /home/guest/work/ABC
