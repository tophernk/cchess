pipeline {
   agent any
   triggers {
        pollSCM('H/15 * * * *')
   }
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
                sh "valgrind --leak-check=full --track-origins=yes ./lib/test/position_test"
                sh "valgrind --leak-check=full --track-origins=yes ./lib/test/piece_test"
                sh "valgrind --leak-check=full --track-origins=yes ./lib/test/move_test"
                //sh "valgrind --leak-check=full --track-origins=yes ./lib/test/config_test"
                //sh "valgrind --leak-check=full --track-origins=yes ./bot/test/ccbot_test"
            }
        }
        stage('Docker EVAL SERVICE') {
            steps {
                sh "docker build -t christopherjunk/evalservice ./services/eval/"
                sh "docker push christopherjunk/evalservice"
                sh "docker run -d --rm -p1025:1025 --network dockernet --name evalservice christopherjunk/evalservice"
            }
        }
        stage('Docker MOVE SERVICE') {
            steps {
                sh "docker build -t christopherjunk/moveservice ./services/move/"
                sh "docker push christopherjunk/moveservice"
                sh "docker run -d --rm -p1024:1024 -v ${env.WORKSPACE}/logs:/moveservice-logs --network dockernet --name moveservice christopherjunk/moveservice"
            }
        }
        stage('Acceptance Test EVAL SERVICE') {
            steps {
                sleep 2
                sh "chmod +x ./services/eval/acceptance_test.sh"
                //sh "./services/eval/acceptance_test.sh"
                sh "./services/eval/evalclient evalservice"
            }
        }
        stage('Acceptance Test MOVE SERVICE') {
            steps {
                sleep 2
                sh "chmod +x ./services/move/acceptance_test.sh"
                //sh "./services/move/acceptance_test.sh"
                sh "./services/move/moveclient moveservice"
            }
        }
    }
    post {
        always {
            sh "docker stop moveservice"
            sh "docker stop evalservice"
        }
    }
}
