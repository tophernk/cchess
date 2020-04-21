pipeline {
   agent any
   stages {
        stage('Checkout') {
            steps {
                // Get code from GitHub repository
                git 'https://github.com/tophernk/cchess.git'
            }
        }
        stage('MAKE') {
            steps {
                // Run CMAKE
                sh "cmake -j 4 ."
                // RUN MAKE
                sh "make"
            }
        }
        stage('TEST') {
            steps {
                // RUN TESTS
                sh "ctest -VV"
            }
        }
    }
}
