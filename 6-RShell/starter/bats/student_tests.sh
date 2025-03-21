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
    ./dsh -s -p 7890 &
    server_pid=$!
    
    sleep 1
    
    run "./dsh" -c -p 7890 <<EOF
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
