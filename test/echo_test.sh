#!/usr/bin/env bash

function cleanup_subprocesses() {
  for pid in $@; do
    kill $pid 2>/dev/null
    wait $pid 2>/dev/null || true
  done
}
export -f cleanup_subprocesses
trap 'cleanup_subprocesses $(jobs -p)' INT TERM EXIT;

function wait_for_server_connection() {
  test_server_host=$1
  test_server_port=$2
  max_seconds_to_wait_for_server_connection=$3

  i=0
  while ! nc $test_server_host $test_server_port </dev/null; do
    sleep 1
    ((i++)) || true
    if [ $i -gt $max_seconds_to_wait_for_server_connection ]; then
      return 1
    fi
  done
  return 0
}

max_wait_seconds=3

function test_successful_echo() {
  test_name=$1
  test_server_host=$2
  test_server_port=$3
  build/echo $test_server_host $test_server_port 2>&1 >/dev/null &
  server_pid=$!

  if ! wait_for_server_connection $test_server_host $test_server_port $max_wait_seconds; then
    echo "[FAIL][Test \"$test_name\"] server startup timed out"
    return 1
  fi

  expected_output=$(echo -e "ohai\nkthxbai")
  actual_output=$(echo -e "ohai\nkthxbai" | nc $test_server_host $test_server_port)

  kill $server_pid
  wait $server_pid

  if [ "$expected_output" == "$actual_output" ]; then
    echo "Test \"$test_name\" passed"
    return 0
  else
    echo "[FAIL][Test \"$test_name\"] expected '$expected_output', but got '$actual_output'"
    return 1
  fi
}

test_successful_echo "ipv4 address" 127.0.0.1 4567
test_successful_echo "ipv6 address" ::1 4567
test_successful_echo "named (non-numeric) address" localhost 4567
