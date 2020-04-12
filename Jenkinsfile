pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                script {
                    sh 'git submodule update --init --recursive'
                    sh 'cmake .'
                    sh 'cmake --build .'
                    sh '/usr/bin/ctest -T test --output-on-failure'
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
            deleteDir()
        }
    }
}