pipeline {
   agent any
   stages {
        stage('Checkout') {
            steps {
                git 'https://github.com/tophernk/cchess.git'
            }
        }
        stage('Make') {
            steps {
                sh "cmake -DGCC=ON -DCLANG=OFF -j 4 ."
                sh "make"
            }
        }
        stage('Test') {
            steps {
                sh "ctest -VV"
            }
        }
        stage('Memory') {
            steps {
                sh "valgrind --leak-check=full ./test/position_test"
                sh "valgrind --leak-check=full ./test/piece_test"
                sh "valgrind --leak-check=full ./test/move_test"
                sh "valgrind --leak-check=full ./test/config_test"
            }
        }
    }
}
