/*
 * Copyright (c) of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>
 * Anshul Gupta <anshulgupta0803@gmail.com>
 * 
 * Modifications are made under the terms of the GNU LGPL license.
 */

import java.io.FileInputStream;
import java.io.BufferedReader;
import java.io.InputStreamReader;

public class Mask {
    public static void main (String[] args) {
	try {
	    FileInputStream fstream = new FileInputStream (args[0]);
	    BufferedReader br = new BufferedReader (new InputStreamReader (fstream));
	    String line;
	    String key;
	    Double threshold = 0.1;
	    Integer current_event = 0;
	    Integer current_subblock = 0;
	    Integer opcode = 0;

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
		    }


	}
	catch (Exception e) {}
    }
}
