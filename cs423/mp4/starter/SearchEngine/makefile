HADOOP_HOME=../hadoop-1.0.4

make:
	javac -classpath $(HADOOP_HOME)/hadoop-core-1.0.4.jar XMLReader.java Identify.java Page.java
	jar -cvmf Identify.mf Identify.jar *.class
	
	javac Formatter.java
	jar -cvmf Formatter.mf Formatter.jar Formatter.class XMLReader.class Page.class
	
	cp Identify.jar ../hadoop-1.0.4
	cp Formatter.jar ../hadoop-1.0.4
clean:
	rm *.class *.jar



