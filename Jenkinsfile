pipeline {
   agent any
   stages {
        stage('Checkout') {
            steps {
                // Get some code from a GitHub repository
                git 'https://github.com/tophernk/cchess.git'
            }
        }
        stage('MAKE') {
            steps {
                // Run CMAKE
                sh "cmake ."
                // RUN MAKE
                sh "make"
            }
        }
        stage('TEST') {
            steps {
                // RUN TESTS
                sh "ctest"
            }
        }
    }
}
