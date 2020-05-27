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
                //sh "valgrind --leak-check=full ./test/config_test"
            }
        }
        stage('Docker') {
            steps {
                sh "sudo docker build -t cchess ."
                sh "sudo docker run -d -p1024:1024 cchess"
                //sh "sudo docker tag cchess localhost:5000/cchess"
            }
        }
        stage('Acceptance Test') {
            steps {
                sleep 3
                sh '''
                    test -n \\"$(./exec/cchessclient | grep response)\\"
                    '''
                //sh "chmod +x test/acceptance_test.sh"
                //sh "./test/acceptance_test"
            }
        }
    }
}
