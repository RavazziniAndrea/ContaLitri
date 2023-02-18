package literscounter;

import java.net.InetSocketAddress;
import arlogg.ARLogg;
import arlogg.ARLogg.Level;
import com.sun.net.httpserver.HttpServer;
import java.io.IOException;
import java.sql.SQLException;
import java.time.LocalDateTime;
import java.util.HashMap;
import java.util.Map;


/**
 * @author Andrea Ravazzini
 * https://github.com/RavazziniAndrea
 */
public class LitersCounter
{
    static final ARLogg log = new ARLogg(LitersCounter.class,Level.DEBUG);
    private static final double BEER_LITERS = 0.4;
    private static final double SHOT_LITERS = 0.3;
    private static final double DRINK_LITERS = 0.4;
    private static final double SUPER_BOTTLE_LITERS = 1;
    private static final double WINE_BOTTLE_LITERS = 0.75;
    private static final double PONG_LITERS = 1.5;
    private static final double GLASS_LITERS = 0.3;
    static double qtyTotDb = 0;
    static double qtyTotOld = 0;
    static double startingOffset; 
    static int brightness = 50;
    
    static PropertiesReader properties;
    
    public static void main(String[] args)
    {
        log.info("RUN: "+LocalDateTime.now().toString().replace("T", " "));
        
        if(!readProperties())
        {
            return;
        }
        setBrightness();
        
        startingOffset = properties.getOffset();
        
        Map<String, String> dbProperties = createDbProperties(properties);
        
        try
        {
            //TODO per alleggerire il calcolo, si dovrebbe calcolare una sola volta la qta_tot basata sui giorni precedendi (che tanto rimane fissa)
            // poi si aggiungono i valori di questa giornata
            //quindi calcolo qta_tot fino adesso, lo tengo come valore fisso tenendo presente la data/ora, po ici vado ad aggiungere tutti i valori 
            //con data/ora > data/ora iniziale, così il calcolo risulta molto più leggero, evita di sommare tutti gli ordini fatti ogni volta
            qtyTotDb = DbControl.getQtyTotFromDb(dbProperties) + startingOffset;
            qtyTotOld = qtyTotDb;
        }
        catch(SQLException ex)
        {
            log.fatal("Can't proceed without qtyTot");
            return;
        }

        DataEsp32 startEsp32Data = new DataEsp32(qtyTotDb, brightness);
        log.info("Starting Esp32Data with values: "+startEsp32Data.getLiters()+" -- "+startEsp32Data.getBrightness());
        
        try{startServer(startEsp32Data);}
        catch(IOException ex)
        {
            log.fatal("Error starting the server. ABORT.");
            System.exit(-2);
        }

        log.info("Server Started. Now waiting...");
        
        Thread t = new Thread(() ->
        {
            while(true)
            {
                try
                {
                    qtyTotDb=DbControl.getQtyTotFromDb(dbProperties) + startingOffset;
                    if(qtyTotOld != qtyTotDb)
                    {
                        qtyTotOld = qtyTotDb;
                        MyHttpHandler.updateData(new DataEsp32(qtyTotDb, brightness));
                        log.info("Refresh qty: "+qtyTotDb);
                    }
                    Thread.sleep(2000); //TODO, I know, its not the best solution, but I was in a hurry :)
                }
                catch(InterruptedException ex)
                {
                    log.error("Error interrupt thread: ",ex.getStackTrace());
                }
                catch(SQLException ex)
                {
                    log.error("Error reading qty", ex.getStackTrace());
                }
            }
        }, "DBControl");
        t.setDaemon(false);
        t.start();
    }
    
    private static void startServer(DataEsp32 dataEsp32) throws IOException
    {
        log.info("Starting server....");
        HttpServer server = HttpServer.create(new InetSocketAddress(properties.getServerPort()), 0);
        server.createContext("/"+properties.getServerLocation(), new literscounter.MyHttpHandler(dataEsp32));
        server.setExecutor(null); // creates a default executor
        log.info("Server started!");
        server.start();        
    }

    private static Map<String,String> createDbProperties(PropertiesReader properties)
    {
        Map<String, String> propMap = new HashMap<>();
        propMap.put("dbQuery",properties.getDbQuery());
        propMap.put("dbIp"   ,properties.getDbIp());
        propMap.put("dbPort" ,properties.getDbPort().toString());
        propMap.put("dbTable",properties.getDbTable());
        propMap.put("dbUser" ,properties.getDbUser());
        propMap.put("dbPsw"  ,properties.getDbPsw());
        return propMap;
    }
    
    private static boolean readProperties()
    {
        properties = new PropertiesReader();
        if(!properties.isReadCorrectly())
        {
            log.error("Error reading .properties file");
            log.error("Abort.");
            return false;
        }
        return true;
    }
    
    private static void setBrightness()
    {
        brightness = properties.getLight();
        brightness = (brightness > 80) ? 80 : brightness;
        brightness = (brightness < 20) ? 20 : brightness;
    }
    
    public static double getLitersBeer()
    {
        return BEER_LITERS;
    }

    public static double getLitersShot()
    {
        return SHOT_LITERS;
    }

    public static double getLitersDrink()
    {
        return DRINK_LITERS;
    }

    public static double getLitersSuperBottle()
    {
        return SUPER_BOTTLE_LITERS;
    }

    public static double getLitersWineBottle()
    {
        return WINE_BOTTLE_LITERS;
    }

    public static double getLitersPong()
    {
        return PONG_LITERS;
    }

    public static double getLitersGlass()
    {
        return GLASS_LITERS;
    }
}
