package droppodview;

import com.fazecast.jSerialComm.SerialPort;
import javafx.animation.RotateTransition;
import javafx.application.Application;
import javafx.beans.value.ChangeListener;
import javafx.beans.value.ObservableValue;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.geometry.Insets;
import javafx.scene.Scene;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.paint.Paint;
import javafx.scene.shape.Rectangle;
import javafx.stage.Stage;
import javafx.util.Duration;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.List;
import java.util.Timer;
import java.util.TimerTask;
import java.util.logging.Level;
import java.util.logging.Logger;
import javafx.application.Platform;

public class DropPodView extends Application {

    //this defines the global representation of the port that's going to be used and the list of ports
    private SerialPort arduinoPort = null;
    private Label portLabel = new Label("Port: ");
    private ObservableList<String> portList;

    //tells if the program is connected to the arduino
    private boolean connected = false;

    //Labels for information of the current status of the package
    //speed to be considered
    //private Label speedLabel = new Label("Speed: ");
    private Label isFalling = new Label("Attached to drone");
    private Label status = new Label("Status: ");

    //the image should represent the final packet
    private ImageView selectedImage = new ImageView();

    //purely estetics
    private Paint black = Color.BLACK;
    private Background rootBackground = new Background(new BackgroundFill(black, CornerRadii.EMPTY, Insets.EMPTY));

    //lists every port available
    private void findPorts() {

        portList = FXCollections.observableArrayList();

        for (SerialPort port : SerialPort.getCommPorts()) {
            portList.add(port.getDescriptivePortName());
        }
        portList.add("Arduino Nano: cu.wchusbserial1410");
    }

    //start the routine
    @Override
    public void start(Stage primaryStage) {

        Timer timer = new Timer();

                TimerTask task = new TimerTask() {
                    public void run() {

                        //runs program
                        // while(true){
                        update();

                        //}
                    }

                };

                timer.scheduleAtFixedRate(task, 0, 100l);
        
        findPorts();
        final ComboBox comboBoxPorts = new ComboBox(portList);
        
        comboBoxPorts.valueProperty().addListener(new ChangeListener<String>() {

            //this is triggered when a value is selected from the combo box
            @Override
            public void changed(ObservableValue<? extends String> observable, String oldValue, String newValue) {
                System.out.println("Connecting");
                disconnectArduino();
                System.out.println(newValue);

                connectArduino(newValue);

                connected = true;

              

                new Thread(() -> doTime(selectedImage)).start();

                // doTime(selectedImage);
            }
        });

        //tries to fetch an image
        try {

            // String path=  DropPodView.class.getClassLoader().getResource("/Users/mathew/Documents/GitHub/DroneDropSystem/JavaFX Transmitter UI/DropPodView/build/classes/droppodview/DropPodWireview.png").getPath();
            // System.out.println(path);
            FileInputStream inputStream = new FileInputStream("/Users/mathew/Documents/GitHub/DroneDropSystem/JavaFX Transmitter UI/DropPodView/build/classes/droppodview/DropPodWireview.png");
            Image image = new Image(inputStream);
            selectedImage.setImage(image);
        } catch (Exception e) {
            Logger.getLogger(DropPodView.class.getName())
                    .log(Level.SEVERE, null, e);
            System.out.println(e.toString());
        }

        //set the color of text labels to white to avoid problems
        portLabel.setTextFill(Color.WHITE);
        isFalling.setTextFill(Color.WHITE);
        status.setTextFill(Color.WHITE);

        StackPane diagram = new StackPane();
        diagram.getChildren().add(selectedImage);

        HBox statusBox = new HBox();
        statusBox.getChildren().addAll(status, isFalling);

        VBox portBox = new VBox();
        portBox.getChildren().addAll(portLabel, comboBoxPorts);
        

        VBox root = new VBox();
        root.getChildren().addAll(portBox, diagram, statusBox);
        root.setBackground(rootBackground);
        root.setSpacing(100);

        Scene scene = new Scene(root, 900, 750);

        primaryStage.setTitle("Drone Package View");
        primaryStage.setScene(scene);
        primaryStage.show();

        if (connected) {
            //runs program
            doTime(selectedImage);
        }
        System.out.println("Setup complete");
    }
    public String append = "";
    public double angle = 0;
    RotateTransition rt = new RotateTransition(Duration.millis(100), selectedImage);
    double testAngle =0;
    double targetAngle =45*Math.random();
    double mult = 1;
    double prog = 1;
    private void update() {
        double speed = Math.abs(Math.sin((prog++/180.0) *Math.PI))*5;
       // System.out.println("Speed: "+speed);
        rt.stop();
       // System.out.println("Angle: "+(-angle));
        //rt.setToAngle((-angle)/10);
        
        if(testAngle>=targetAngle && mult ==1){
            targetAngle = Math.random() *-45;
            mult =-1;
        }
        else if (testAngle<targetAngle && mult ==-1){
            targetAngle = Math.random() *45;
            mult = 1;
        }
        
        
        rt.setToAngle((testAngle+=(mult*(speed*Math.random()))));
       // System.out.println("Test Angle: "+testAngle);
        rt.play();
    }

    private void doTime(ImageView selectedImage) {

        //set any rotation regarding the rectangle to take about 1/4 of a second
        //Stream to of symbols from the arduino to read
        InputStream stream = arduinoPort.getInputStream();

        //Sequence to read from the serial monitor
        try {
            //checks if the stream was empty or not
            while (true) {
                while (stream.available() > 0) {

                    int value;
                    value = stream.read();
                    if (value == ';') {
                       
                        String arr[] = append.split(",");

                        // Here are your values
                        List<Double> listOfInformation = new ArrayList<>();

                        try {
                            for (String s : arr) {
                                listOfInformation.add(Double.parseDouble(s));
                            }

                            //the first item that is being sent is the position
                            // System.out.println("Angle: "+angle);
                            //checks for negative representation of anlges
                            //if falling return 1 else return 0
                            if (listOfInformation.get(1) == 1) {

                                //Platform.runLater(()->isFalling.setText("Angle: "+(-angle)));
                                angle = listOfInformation.get(0);
                                // selectedImage.setRotate(angle);
                            }

                            //  rotateTransition.setToAngle(angle);
                        } catch (Exception e) {

                        }
                        append = "";

                    } else {

                        append += (char) value;
                    }

                    //  rotateTransition.setToAngle(0);
                   
                }
            }
        } catch (Exception e) {

        }

    }

    //this connects the program with the arduino in order to read the serial monitor
    private void connectArduino(String port) {

        SerialPort serialPort = getPort(port);

        //error catching if any problem should arise during connection
        try {

            //finally opens the connection
            serialPort.openPort();
            serialPort.setBaudRate(9600);
            arduinoPort = serialPort;
        } catch (Error ex) {

            Logger.getLogger(DropPodView.class.getName())
                    .log(Level.SEVERE, null, ex);
            System.out.println("SerialPortException: " + ex.toString());
        }
    }

    //finds the index of the port that the user selected and returns the port
    private SerialPort getPort(String port) {

        for (SerialPort currentPort : SerialPort.getCommPorts()) {
            if (currentPort.getDescriptivePortName().equals(port)) {
                return currentPort;
            }
        }
        //if something goes wrong null is returned
        return null;
    }

    //when the program closes or when the interface is closed so is the connection to the arduino
    private void disconnectArduino() {

        if (arduinoPort != null) {
            try {
                arduinoPort.removeDataListener();
                arduinoPort.closePort();

                //if this line is printed it means that the arduino was disconnected successfully
                System.out.println("disconnectArduino()");

            } catch (Error ex) {
                Logger.getLogger(DropPodView.class.getName())
                        .log(Level.SEVERE, null, ex);
            }
        }
    }

    //function called when the program/user interface is closed
    @Override
    public void stop() throws Exception {
        disconnectArduino();
        super.stop();
    }

    public static void main(String[] args) {
        launch(args);
    }
}
