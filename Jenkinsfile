pipeline {
    agent any
    stages {
        stage('Init') {
            steps {
                script {
                    sh 'git submodule update --init --recursive'
                }
            }
        }
        stage('Build') {
            steps {
                script {
                    sh 'cmake .'
                    sh 'cmake --build .'
                }
            }
        }
        stage('Test') {
            steps {
                script {
                    sh '/usr/bin/ctest -T test --verbose'
                }
            }
        }
    }
	post {
        always {
            xunit (
                testTimeMargin: '3000',
                thresholdMode: 1,
                thresholds: [
                  skipped(failureThreshold: '0'),
                  failed(failureThreshold: '0')
                ],
                tools: [CTest(
                    pattern: 'Testing/**/*.xml',
                    deleteOutputFiles: true,
                    failIfNotNew: false,
                    skipNoTestFiles: true,
                    stopProcessingIfError: true
                  )]
            )
            //deleteDir()
        }
    }
}