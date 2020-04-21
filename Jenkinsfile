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
                sh "mkdir -p build"
                sh "cd build"
                // Run CMAKE
                sh "cmake .."
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
