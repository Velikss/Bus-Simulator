pipeline {
    agent none
    parallel
    {
        stages
        {
            stage('Init Linux')
            {
                agent
                {
                    label "Linux"
                }
                steps
                {
                    script
                    {
                        sh 'git submodule update --init --recursive'
                    }
                }
            }
            stage('Init Windows')
            {
                agent
                {
                    label "Windows"
                }
                steps
                {
                    script
                    {
                        bat 'git submodule update --init --recursive'
                    }
                }
            }
        }
    }
    parallel
    {
        stages
        {
            stage('Build Linux')
            {
                agent
                {
                    label "Linux"
                }
                steps
                {
                    script
                    {
                        sh 'cmake .'
                        sh 'cmake --build .'
                    }
                }
            }
            stage('Build Windows')
            {
                agent
                {
                    label "Windows"
                }
                steps
                {
                    script
                    {
                        bat 'cmake .'
                        bat 'cmake --build .'
                    }
                }
            }
        }
    }
    parallel
    {
        stages
        {
            stage('Test Linux')
            {
                agent
                {
                    label "Linux"
                }
                steps
                {
                    script
                    {
                        sh '/usr/bin/ctest -T test --verbose'
                    }
                }
            }
            stage('Test Windows')
            {
                agent
                {
                    label "Windows"
                }
                steps
                {
                    script
                    {
                        bat 'ctest -T test --verbose'
                    }
                }
            }
        }
    }
}