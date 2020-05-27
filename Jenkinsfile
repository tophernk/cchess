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
        stage('Docker') {
            steps {
                sh "sudo docker build -t christopherjunk/cchess ."
                sh "sudo docker push christopherjunk/cchess"
                sh "sudo docker run -d --rm -p1024:1024 --network dockernet --name cchess cchess"
            }
        }
        stage('Acceptance Test') {
            steps {
                sleep 2
                sh "chmod +x ./test/acceptance_test.sh"
                sh "./test/acceptance_test.sh"
            }
        }
    }
    post {
        always {
            sh "sudo docker stop cchess"
        }
    }
}
