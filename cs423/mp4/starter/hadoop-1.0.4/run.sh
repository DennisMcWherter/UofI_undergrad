rm -rf output

java -jar Formatter.jar ../../data/1.xml ./input
bin/hadoop jar Identify.jar input output
