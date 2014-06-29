/*
 * Copyright (c) of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>
 * Anshul Gupta <anshulgupta0803@gmail.com>
 * 
 * Modifications are made under the terms of the GNU LGPL license.
 */

/*
 * Parse the global values and individual bins into seperate variables.
 * Use appropriate System.out.println/printf to get the desired format.
 * 
 * For global variables:
 * (<global_events> <global_average> <global_min_rank> <global_minimum> <global_max_rank> <global_maximum>)
 * 
 * For i-th bin:
 * {<bin_begin[i]> <bin_minimum[i]> <bin_average[i]> <bin_maximum[i]> <bin_end[i]> <bin_events[i]>}
 * 
 * For the processed global values:
 * (<new_global_events> <new_global_average> <new_global_min_rank> <new_global_minimum> <new_global_max_rank> <new_global_maximum>)
 * new_global_events = global_events
 * new_global_min_rank = global_min_rank	(cannot determine the new min_rank)
 * new_global_max_rank = global_max_rank	(cannot determine the new max_rank)
 */

import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Expand {
    public static void main (String[] args) {
	try {
	    FileInputStream fstream = new FileInputStream (args[0]);
	    BufferedReader br = new BufferedReader (new InputStreamReader (fstream));
	    String line;
	    String key;
	    Double threshold = 0.5;
	    Integer current_event = 0;
	    Integer current_subblock = 0;
	    Integer opcode = 0;

	    System.out.println ("eventID opcode subblock events avg min max");
	    while ((line = br.readLine ()) != null) {
		if (line.length() > 7) {
		    key = line.substring (0, 7);
		    
		    if (key.startsWith ("EVENT"))
			key = key.substring (0, 5);
		    
		    switch (key) {
   		        case "EVENT":
			    /* Parse the Event ID */
			    current_event = Integer.parseInt (line.substring (5, line.indexOf (':')));
			    /* Parse the opcode */
			    opcode = Integer.parseInt (line.substring (line.indexOf (' ') + 1, line.indexOf (' ', line.indexOf (':') + 2)));
			    break;

			/* Can be changed to COMP 4c, COMP 4b or COMM 4b */
  		        case "COMP 4b":
			    /* Parse the subblock ID */
			    current_subblock = Integer.parseInt (line.substring (8, line.indexOf (':')));

			    /* Parse the global tuple */
			    String global = line.substring (line.indexOf ('(') + 1, line.indexOf (')'));
			    String[] global_variables = global.split (" ");
			    Integer global_events = Integer.parseInt (global_variables[0]);
			    Double global_average = Double.parseDouble (global_variables[1]);
			    Integer global_min_rank = Integer.parseInt (global_variables[2]);
			    Double global_minimum = Double.parseDouble (global_variables[3]);
			    Integer global_max_rank = Integer.parseInt (global_variables[4]);
			    Double global_maximum = Double.parseDouble (global_variables[5]);

			    Double[] bin_begin = new Double[5];
			    Double[] bin_minimum = new Double[5];
			    Double[] bin_average = new Double[5];
			    Double[] bin_maximum = new Double[5];
			    Double[] bin_end = new Double[5];
			    Integer[] bin_events = new Integer[5];

			    /* Parse bin1 */
			    String bin1 = line.substring (line.indexOf ('{') + 1, line.indexOf ('}'));
			    String[] bin1_variables = bin1.split (" ");
			    bin_begin[0] = Double.parseDouble (bin1_variables[0]);
			    bin_minimum[0] = Double.parseDouble (bin1_variables[1]);
			    bin_average[0] = Double.parseDouble (bin1_variables[2]);
			    bin_maximum[0] = Double.parseDouble (bin1_variables[3]);
			    bin_end[0] = Double.parseDouble (bin1_variables[4]);
			    bin_events[0] = Integer.parseInt (bin1_variables[5]);

			    /*Parse bin2 */
			    String bin2 = line.substring (line.indexOf ('{', line.indexOf ('{') + 1) + 1, line.indexOf ('}', line.indexOf ('}') + 1));
			    String[] bin2_variables = bin2.split (" ");
			    bin_begin[1] = Double.parseDouble (bin2_variables[0]);
			    bin_minimum[1] = Double.parseDouble (bin2_variables[1]);
			    bin_average[1] = Double.parseDouble (bin2_variables[2]);
			    bin_maximum[1] = Double.parseDouble (bin2_variables[3]);
			    bin_end[1] = Double.parseDouble (bin2_variables[4]);
			    bin_events[1] = Integer.parseInt (bin2_variables[5]);

			    /* Parse bin3 */
			    String bin3 = line.substring (line.indexOf ('{', line.indexOf ('{', line.indexOf ('{') + 1) + 1) + 1, line.indexOf ('}', line.indexOf ('}', line.indexOf ('}') + 1) + 1));
			    String[] bin3_variables = bin3.split (" ");
			    bin_begin[2] = Double.parseDouble (bin3_variables[0]);
			    bin_minimum[2] = Double.parseDouble (bin3_variables[1]);
			    bin_average[2] = Double.parseDouble (bin3_variables[2]);
			    bin_maximum[2] = Double.parseDouble (bin3_variables[3]);
			    bin_end[2] = Double.parseDouble (bin3_variables[4]);
			    bin_events[2] = Integer.parseInt (bin3_variables[5]);

			    /* Parse bin4 */
			    String bin4 = line.substring (line.indexOf ('{', line.indexOf ('{', line.indexOf ('{', line.indexOf ('{') + 1) + 1) + 1) + 1, line.indexOf ('}', line.indexOf ('}', line.indexOf ('}', line.indexOf ('}') + 1) + 1) + 1));
			    String[] bin4_variables = bin4.split (" ");
			    bin_begin[3] = Double.parseDouble (bin4_variables[0]);
			    bin_minimum[3] = Double.parseDouble (bin4_variables[1]);
			    bin_average[3] = Double.parseDouble (bin4_variables[2]);
			    bin_maximum[3] = Double.parseDouble (bin4_variables[3]);
			    bin_end[3] = Double.parseDouble (bin4_variables[4]);
			    bin_events[3] = Integer.parseInt (bin4_variables[5]);

			    /* Parse bin5 */
			    String bin5 = line.substring (line.indexOf ('{', line.indexOf ('{', line.indexOf ('{', line.indexOf ('{', line.indexOf ('{') + 1) + 1) + 1) + 1) + 1, line.indexOf ('}', line.indexOf ('}', line.indexOf ('}', line.indexOf ('}', line.indexOf ('}') + 1) + 1) + 1) + 1));
			    String[] bin5_variables = bin5.split (" ");
			    bin_begin[4] = Double.parseDouble (bin5_variables[0]);
			    bin_minimum[4] = Double.parseDouble (bin5_variables[1]);
			    bin_average[4] = Double.parseDouble (bin5_variables[2]);
			    bin_maximum[4] = Double.parseDouble (bin5_variables[3]);
			    bin_end[4] = Double.parseDouble (bin5_variables[4]);
			    bin_events[4] = Integer.parseInt (bin5_variables[5]);

			    Double sum = 0.0;
			    Integer events = 0;
			    Double minimum = Integer.MAX_VALUE * 1.0;
			    Double maximum = Integer.MIN_VALUE * 1.0;

			    /* Recalculate the global averages on the basis of threshold set above */
			    for (int i = 0; i < 5; i++) {
				if (global_average > 100 && bin_events[i] >= global_events * threshold) {
				    sum += bin_average[i] * bin_events[i];
				    events += bin_events[i];

				    if (bin_minimum[i] <= minimum)
					minimum = bin_minimum[i];

				    if (bin_maximum[i] >= maximum)
					maximum = bin_maximum[i];
				}
				// System.out.printf ("%d %d %d %.2f %.2f %.2f\n", current_event, current_subblock, bin_events[i], bin_minimum[i], bin_average[i], bin_maximum[i]);
			    }

			    Integer new_global_events = global_events;
			    Double new_global_average = sum / events;
			    Integer new_global_min_rank = global_min_rank;
			    Double new_global_minimum = minimum;
			    Integer new_global_max_rank = global_max_rank;
			    Double new_global_maximum = maximum;

			    System.out.println (current_event + " " + opcode + " " + current_subblock + " " + global_events + " " + global_average + " " + global_minimum + " " + global_maximum);
			    // System.out.printf("%d %d %d %.2f %.2f\n", current_event, current_subblock, bin_events[i], bin_minimum[i], bin_average[i], bin_maximum[i]);
			    break;
			    
		        default:
			    break;
		    }
		}
	    }
	    br.close ();
	}
	catch (Exception e) {
	    System.out.println (e);
	}
	finally {}
    }
}
