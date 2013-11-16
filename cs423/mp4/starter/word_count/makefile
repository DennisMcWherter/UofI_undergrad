HADOOP_HOME=../hadoop-1.0.4

make:
	javac -classpath $(HADOOP_HOME)/hadoop-core-1.0.4.jar WordCount.java 
	jar cvf WordCount.jar *.class

clean:
	rm *.class WordCount.jar



