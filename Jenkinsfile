pipeline {
   agent any
   stages {
        stage('Checkout') {
            steps {
                git 'https://github.com/tophernk/cchess.git'
            }
        }
        stage('MAKE') {
            steps {
                sh "cmake -DGCC=ON -DCLANG=OFF -j 4 ."
                sh "make"
            }
        }
        stage('TEST') {
            steps {
                sh "ctest -VV"
            }
        }
        stage('MEMORY') {
            steps {
                sh "valgrind --leak-check=full ./test/position_test"
                sh "valgrind --leak-check=full ./test/piece_test"
                sh "valgrind --leak-check=full ./test/config_test"
            }
        }
    }
}
