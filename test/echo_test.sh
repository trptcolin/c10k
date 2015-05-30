#!/usr/bin/env bash

set -e

test_server_host="127.0.0.1"
test_server_port=12345

function cleanup_subprocesses() {
  for pid in $@; do
    kill $pid 2>/dev/null
    wait $pid 2>/dev/null || true
  done
}
export -f cleanup_subprocesses
trap 'cleanup_subprocesses $(jobs -p)' EXIT;

function wait_for_server_connection() {
  max_seconds_to_wait_for_server_connection=$1

  i=0
  while ! nc $test_server_host $test_server_port </dev/null; do
    sleep 1
    ((i++)) || true
    if [ $i -gt $max_seconds_to_wait_for_server_connection ]; then
      echo
      echo "[FAIL] ** Server connection timed out"
      exit 1
    fi
  done
}

build/echo $test_server_host $test_server_port 2>&1 >/dev/null &

wait_for_server_connection 3

expected_output=$(echo -e "ohai\nkthxbai")
actual_output=$(echo -e "ohai\nkthxbai" | nc $test_server_host $test_server_port)

if [ "$expected_output" == "$actual_output" ]; then
  echo Pass
else
  echo "[FAIL] expected '$expected_output', but got '$actual_output'"
fi
