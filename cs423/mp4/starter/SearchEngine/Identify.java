import java.io.IOException;
import java.util.*;
import java.util.Map.Entry;

import org.apache.hadoop.fs.Path;
import org.apache.hadoop.conf.*;
import org.apache.hadoop.io.*;
import org.apache.hadoop.mapred.*;
import org.apache.hadoop.util.*;

public class Identify {

	public static class Map extends MapReduceBase implements
			Mapper<LongWritable, Text, Text, Text> {
		
		@Override
		public void map(LongWritable key, Text line, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {
			
		    String lineString = line.toString();
		
            int firstSpace = lineString.indexOf(" ");
			String id = lineString.substring(0, firstSpace);
			String noIdLine = lineString.substring(firstSpace+1);
			StringTokenizer tokens = new StringTokenizer(noIdLine);
				
			while(tokens.hasMoreTokens()){
				String token = tokens.nextToken();
				output.collect(new Text(token), new Text(id));
			}
        }
		
	}

	
	
	public static class Reduce extends MapReduceBase implements
			Reducer<Text, Text, Text, Text> {

		@Override
		public void reduce(Text key, Iterator<Text> values, OutputCollector<Text, Text> output, Reporter reporter) throws IOException {
			
			HashMap<Integer, Integer> map = new HashMap<Integer, Integer>(); 
            StringBuilder out = new StringBuilder();

			while (values.hasNext()){
				int temp = Integer.parseInt((values.next().toString()));

				if (map.containsKey(temp)){
					map.put(temp, map.get(temp)+1);
				}else{
					map.put(temp, 1);
				}
			}
			
			for (Entry<Integer, Integer> entry : map.entrySet()){
				out.append(entry.getKey() + "-" + entry.getValue() + " ");
			}
			
			output.collect(key, new Text(out.toString()));
		}
	}

	
	
	public static void main(String[] args) throws Exception {
		
		//XMLReader x = new XMLReader(args[0]);
		//x.writeNewFile("input/flat.txt");
		
		JobConf conf = new JobConf(Identify.class);
		conf.setJobName("identify");

		conf.setOutputKeyClass(Text.class);
		conf.setOutputValueClass(Text.class);

		conf.setMapperClass(Map.class);
        //NOTE: Idea. Perhaps set combiner combines fragmented
        // keys - do we need to create a new one??
		//conf.setCombinerClass(Reduce.class);
		conf.setReducerClass(Reduce.class);

		conf.setInputFormat(TextInputFormat.class);
		conf.setOutputFormat(TextOutputFormat.class);

		FileInputFormat.setInputPaths(conf, new Path(args[0]));
		FileOutputFormat.setOutputPath(conf, new Path(args[1]));

		JobClient.runJob(conf);
	}
}
