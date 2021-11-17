#!/bin/sh
name=$1
base=${name##*.}
header=(${@:2})
cat << -
: verify   = "*>^~64.4"
: tag      = "kiya"
: version  = "kirakira"
: author   = "vya"
[$] "description" @text => @"desc.txt"
: desc     = "description"

[] "kiya.export" @"kiya.export.all"

[$ "kiya"]          "name"        @string => "${name}"
[$ "kiya" "parse"]  "kiya.export" @null
[$ "kiya" "dylink" "lib${base}.so"]
-
for h in ${header[@]}; do
	printf "\t(\"import-api-by-c-header\" \"/usr/local/include/$h\")\n"
done
