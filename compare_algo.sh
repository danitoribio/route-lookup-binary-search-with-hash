#!/usr/bin/env bash

ips_ls=( )
outInterfaces_ls=( )
times_ls=( )
i=0

while IFS=';' read -r ip outInterface tableAccesses time; do
    if [ -z "$ip" ] &&  [ $i -ne -1 ]; then
        ((i=-1))
        echo "Summary Multibit"
    fi

    if [ $i -eq -1 ]; then
        echo $ip
    fi
done < $1

((i=0))

while IFS=';' read -r ip outInterface tableAccesses time; do
    if [ -z "$ip" ] &&  [ $i -ne -1 ]; then
        ((i=-1))
        echo "Summary Linear Search"
    fi

    if [ $i -eq -1 ]; then
        echo $ip
    else
        ips_ls+=("$ip")
        outInterfaces_ls+=("$outInterface")
        times_ls+=("$time")
    fi
done < $2

((i=0))

while IFS=';' read -r ip outInterface tableAccesses time; do
    if [ -z "$ip" ] && [ $i -ne -1 ]; then
        ((i=-1))
        echo " "
        echo "Summary Algo"
    fi

    if [ $i -eq -1 ]; then
        echo $ip
    else
        if [ "$ip" == "${ips_ls[i]}" ]; then
            if [ "$outInterface" != "${outInterfaces_ls[i]}" ]; then
                echo "Error in ${ip}: out ${outInterface} (correct ${outInterfaces_ls[i]})"
            fi
        fi
        ((i=i+1))
    fi

done < $3
