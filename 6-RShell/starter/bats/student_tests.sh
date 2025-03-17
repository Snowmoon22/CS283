#!/usr/bin/env bats

@test "Local Mode: Basic Command" {
    run "./dsh" <<EOF
ls -la
exit
EOF

    echo "Captured stdout:"
    echo "$output"
    echo "Exit Status: $status"
    
    [[ "$output" == *"local mode"* ]]
    [[ "$output" == *"dshlib.c"* ]]
    [[ "$output" == *"rshlib.h"* ]]
    
    [ "$status" -eq 0 ]
}

@test "Local Mode: Pipe Command" {
    run "./dsh" <<EOF
ls | grep .c
exit
EOF

    echo "Captured stdout:"
    echo "$output"
    echo "Exit Status: $status"
    
    [[ "$output" == *"local mode"* ]]
    [[ "$output" == *"dshlib.c"* ]]
    [[ "$output" == *"dsh_cli.c"* ]]
    
    [ "$status" -eq 0 ]
}

@test "Local Mode: Built-in Command (cd)" {
    run "./dsh" <<EOF
cd ..
pwd
exit
EOF

    echo "Captured stdout:"
    echo "$output"
    echo "Exit Status: $status"
    
    [[ "$output" == *"local mode"* ]]
    [[ "$output" != *"$(basename $(pwd))"* ]]
    
    [ "$status" -eq 0 ]
}

@test "Server Mode: Start and Stop" {
    # Start the server in the background
    ./dsh -s -p 5678 &
    server_pid=$!
    
    # Give the server time to start
    sleep 1
    
    # Run the client and send the 'stop-server' command
    run ./dsh -c -p 5678 <<EOF
stop-server
EOF

    # Wait for the server to stop
    wait $server_pid
    server_status=$?
    
    echo "Server exit status: $server_status"
    echo "Client output:"
    echo "$output"
    echo "Client exit status: $status"
    
    # Validate server and client behavior
    [ "$server_status" -eq 0 ]
    [ "$status" -eq 0 ]
    [[ "$output" == *"stop"* ]]
}

@test "Server Mode: Basic Command Execution (echo)" {
    # Start the server in the background
    ./dsh -s -p 5678 &
    server_pid=$!
    
    # Give the server time to start
    sleep 1
    
    # Run the client and send the 'echo' command
    run ./dsh -c -p 5678 <<EOF
echo Hello World
exit
EOF

    # Stop the server
    run ./dsh -c -p 5678 <<EOF
stop-server
EOF

    # Wait for the server to stop
    wait $server_pid
    server_status=$?
    
    echo "Server exit status: $server_status"
    echo "Client output:"
    echo "$output"
    echo "Client exit status: $status"
    
    # Validate server and client behavior
    [ "$server_status" -eq 0 ]
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello World"* ]]
}


@test "Server Mode: Invalid Command" {
    # Start the server in the background
    ./dsh -s -p 5678 &
    server_pid=$!
    
    # Give the server time to start
    sleep 1
    
    # Run the client and send an invalid command
    run ./dsh -c -p 5678 <<EOF
invalid_command
exit
EOF

    # Stop the server
    run ./dsh -c -p 5678 <<EOF
stop-server
EOF

    # Wait for the server to stop
    wait $server_pid
    server_status=$?
    
    echo "Server exit status: $server_status"
    echo "Client output:"
    echo "$output"
    echo "Client exit status: $status"
    
    [ "$server_status" -eq 0 ]
    [ "$status" -eq 0 ]
    [[ "$output" == *"rdsh-error: command execution error"* ]]
}

@test "Server Mode: Large Command Output" {
    # Start the server in the background
    ./dsh -s -p 5678 &
    server_pid=$!
    
    # Give the server time to start
    sleep 1
    
    # Run the client and send a command that generates large output
    run ./dsh -c -p 5678 <<EOF
cat /dev/urandom | head -c 100000
exit
EOF

    # Stop the server
    run ./dsh -c -p 5678 <<EOF
stop-server
EOF

    # Wait for the server to stop
    wait $server_pid
    server_status=$?
    
    echo "Server exit status: $server_status"
    echo "Client output length:"
    echo "${#output}"
    echo "Client exit status: $status"
    
    [ "$server_status" -eq 0 ]
    [ "$status" -eq 0 ]
    [ "${#output}" -eq 100000 ]
}