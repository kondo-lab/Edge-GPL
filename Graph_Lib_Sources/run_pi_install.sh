BOOST_FOLDER=/opt/boost_1_67_0/
METIS_FOLDER=/opt/metis-5.1.0
if [ ! -d $BOOST_FOLDER ]; then
    # ref: https://www.osetc.com/en/how-to-install-boost-on-ubuntu-16-04-18-04-linux.html
    wget https://boostorg.jfrog.io/artifactory/main/release/1.67.0/source/boost_1_67_0.tar.gz
    tar xf boost_1_67_0.tar.gz
    mv boost_1_67_0 /opt
    cd $BOOST_FOLDER
    ./bootstrap.sh
    ./b2
    echo "export LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:/opt/boost_1_67_0/stage/lib" >> ~/.bashrc
    source ~/.bashrc
fi
if [ ! -d $METIS_FOLDER ]; then
    wget http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz
    tar xf metis-5.1.0.tar.gz
    rm metis-5.1.0.tar.gz
    mv metis-5.1.0 /opt
    cd $METIS_FOLDER
    make config
    make -j4
    cp $METIS_FOLDER/build/*/libmetis/libmetis.a $METIS_FOLDER
fi

apt-get install -y graphviz
apt-get install -y libssl-dev
echo "Boost installed in $BOOST_FOLDER"
# apt-get install libboost-dev
### The result of command ./b2
# The Boost C++ Libraries were successfully built!
# The following directory should be added to compiler include paths:
#     /opt/boost_1_67_0
# The following directory should be added to linker library paths:
#     /opt/boost_1_67_0/stage/lib

# How to compile
#  g++ -I /opt/boost_1_67_0 <all your cpp file with relative path> -o <result file> -pthread -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph


# Example
# g++ -pthread -I /opt/boost_1_67_0 ./Server/main.cpp ./CommonLib/socket.cpp ./CommonLib/threads.cpp ./CommonLib/graph.cpp ./CommonLib/graphstructure.cpp ./CommonLib/graphalgorithms.cpp -o server -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph
# g++ -pthread -I /opt/boost_1_67_0 ./Client/main.cpp ./CommonLib/socket.cpp ./CommonLib/threads.cpp ./CommonLib/graph.cpp ./CommonLib/graphstructure.cpp ./CommonLib/graphalgorithms.cpp -o client -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph

# How to compile modules. Note that the module is compiled into 'client.o'
# g++ -I /opt/boost_1_67_0 ./Client/Modules/main.cpp ./Client/Modules/<module>.cpp -o client -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph

# Example
# g++ -I /opt/boost_1_67_0 ./Client/Modules/main.cpp ./Client/Modules/add_vertex.cpp -o client -L /opt/boost_1_67_0/stage/lib -lboost_regex -lboost_graph