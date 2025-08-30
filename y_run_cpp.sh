#!/usr/bin/env bash
set -eEu -o pipefail
shopt -s xpg_echo

y_run_cpp() {
	if [[ $# -lt 1 ]]; then echo "-- File required"; return; fi
    local filepath=$(realpath $1); shift
	local bin_name=$(mktemp -u /tmp/XXXXXXXXXX)

	g++ --std=c++20 $filepath -o $bin_name && "$bin_name"

	if [[ -f "$bin_name" ]]; then
		rm "$bin_name"
	fi
}

y_run_cpp $*
