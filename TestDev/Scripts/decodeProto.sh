#/bin/bash
# Convert a hex string (in "packed fomat") from an strace dump to raw protobuf fields
echo "$@"|sed "s/\\\x//g"|xxd -r -p|protoc --decode_raw
