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
                sh "valgrind --leak-check=full ./lib/test/position_test"
                sh "valgrind --leak-check=full ./lib/test/piece_test"
                sh "valgrind --leak-check=full ./lib/test/move_test"
                sh "valgrind --leak-check=full ./lib/test/config_test"
                sh "valgrind --leak-check=full ./bot/test/ccbot_test"
            }
        }
        stage('Docker EVAL SERVICE') {
            steps {
                sh "sudo docker build -t christopherjunk/evalservice ./services/eval/"
                sh "sudo docker push christopherjunk/evalservice"
                sh "sudo docker run -d --rm -p1025:1025 --network dockernet --name evalservice christopherjunk/evalservice"
            }
        }
        stage('Docker MOVE SERVICE') {
            steps {
                sh "sudo docker build -t christopherjunk/moveservice ./services/move/"
                sh "sudo docker push christopherjunk/moveservice"
                sh "sudo docker run -d --rm -p1024:1024 --network dockernet --name moveservice christopherjunk/moveservice"
            }
        }
        stage('Acceptance Test EVAL SERVICE') {
            steps {
                sleep 2
                sh "chmod +x ./services/eval/acceptance_test.sh"
                sh "./services/eval/acceptance_test.sh"
            }
        }
        stage('Acceptance Test MOVE SERVICE') {
            steps {
                sleep 2
                sh "chmod +x ./services/move/acceptance_test.sh"
                sh "./services/move/acceptance_test.sh"
            }
        }
    }
    post {
        always {
            sh "sudo docker stop moveservice"
            sh "sudo docker stop evalservice"
        }
    }
}
