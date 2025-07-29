# Function :
    - Peer client & server. 
    - Connects with discovery server to transmit self information & fetch another peer information.
    - Starts a TCP server on host machine such that another peers can establish direct connection after 
        fetching information from discovery server.


# Building :
    - Make run.sh as executable with : chmod +x ./run.sh
    - Start executable with ./run.sh ( This will re-build on change & start the build )
    
# Clean : 
    make clean ( Remove the build directory )
