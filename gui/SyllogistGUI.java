import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

public class SyllogistGUI {

    private static JTextArea inputArea;
    private static JLabel statusLabel;

    public static void main(String[] args) {
        JFrame frame = new JFrame("Syllogist");
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setSize(800, 600);
        frame.setLayout(new BorderLayout());

        // Input area with larger font size
        inputArea = new JTextArea();
        inputArea.setFont(new Font("Monospaced", Font.PLAIN, 22));  // Larger font size
        inputArea.setLineWrap(true);
        inputArea.setWrapStyleWord(true);
        inputArea.setPreferredSize(new Dimension(800, 400));  // Resize the input area

        inputArea.setMargin(new Insets(15, 15, 15, 15)); // Top, left, bottom, right padding

        JScrollPane scrollPane = new JScrollPane(inputArea);
        scrollPane.setBorder(BorderFactory.createLineBorder(Color.GRAY));

        // Run button with larger size and padding
        JButton runButton = new JButton("Run");
        runButton.setFont(new Font("Arial", Font.PLAIN, 18));  // Larger font for the button
        runButton.setPreferredSize(new Dimension(120, 40));  // Make button bigger
        runButton.setFocusPainted(false);
        runButton.setBorder(BorderFactory.createEmptyBorder(10, 20, 10, 20)); // Add padding

        runButton.addActionListener(e -> runSyllogist());

        // Status label with larger font size
        statusLabel = new JLabel(" ");
        statusLabel.setFont(new Font("Arial", Font.PLAIN, 18));  // Larger font for the status label

        // Adding padding and spacing to the bottom panel
        JPanel bottomPanel = new JPanel(new BorderLayout());
        bottomPanel.setBorder(BorderFactory.createEmptyBorder(10, 10, 10, 10));  // Add padding to the bottom panel
        bottomPanel.add(runButton, BorderLayout.WEST);
        bottomPanel.add(statusLabel, BorderLayout.CENTER);

        // Add the scroll pane and bottom panel to the frame
        frame.getContentPane().add(scrollPane, BorderLayout.CENTER);
        frame.getContentPane().add(bottomPanel, BorderLayout.SOUTH);
        frame.setVisible(true);
    }

    private static void runSyllogist() {
        try {
            File inputFile = new File("gui_input.txt");
            File outputDir = new File("output");
            outputDir.mkdir();

            // Write input to gui_input.txt
            try (FileWriter writer = new FileWriter(inputFile)) {
                writer.write(inputArea.getText());
            }

            String[] command = System.getProperty("os.name").startsWith("Windows")
                    ? new String[]{"syllogist.exe", "gui_input.txt"}
                    : new String[]{"./syllogist", "gui_input.txt"};

            ProcessBuilder builder = new ProcessBuilder(command);
            builder.redirectErrorStream(true);

            Process process = builder.start();

            // Optional: read output
            try (BufferedReader reader = new BufferedReader(new InputStreamReader(process.getInputStream()))) {
                String line;
                while ((line = reader.readLine()) != null) {
                    System.out.println(line); // print to console
                }
            }

            int exitCode = process.waitFor();

            switch (exitCode) {
                case 0:
                    statusLabel.setText(" Argument is valid. Wrote Aris file to output directory.");
                    break;
                case 1:
                    statusLabel.setText(" Argument is invalid. Wrote Willow file to output directory.");
                    break;
                case 2:
                    statusLabel.setText(" Parsing error occurred...");
                    break;
                default:
                    statusLabel.setText(" Unknown error occurred...");
            }

        } catch (Exception ex) {
            statusLabel.setText("⚠️ Exception: " + ex.getMessage());
            ex.printStackTrace();
        }
    }
}
