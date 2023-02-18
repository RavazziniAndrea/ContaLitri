package literscounter;

import arlogg.ARLogg;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.Properties;

/**
 * @author Andrea Ravazzini
 * https://github.com/RavazziniAndrea
 */
public class PropertiesReader
{
    private static final ARLogg log = new ARLogg(PropertiesReader.class, ARLogg.Level.DEBUG);

    private final String FILE_PATH = System.getProperty("user.dir") + File.separator + "config.properties";

    private Double  offset;         //offset to correct the liters counter
    private Integer light;          //brightness for the led strips
    private String  dbQuery;        //query to read the data from postgres
    private String  dbIp;           //db ip address
    private Integer dbPort;         //db port
    private String  dbTable;        //db table name
    private String  dbUser;         //username to access the db
    private String  dbPsw;          //password for the specified user
    private String  serverLocation; //server location, aka the path after the port (this)
    private Integer serverPort;     //server port 
    
    private boolean checkFileReadCorrectly=false;
    
    public PropertiesReader()
    {
        readProperties();
    }
    
    private void readProperties()
    {
        try(InputStream input = new FileInputStream(FILE_PATH))
        {
            log.debug("Reading file: "+FILE_PATH);
                
            Properties prop = new Properties();
            prop.load(input);
            try{offset      = Double.parseDouble(prop.getProperty("offset"));}
            catch(NumberFormatException ex)
            {
                log.warn("Cannot read Offset, set to 0");
                offset      = Double.valueOf(0);
            }
            try{light = Integer.parseInt(prop.getProperty("light"));}
            catch(NumberFormatException ex)
            {
                log.warn("Cannot read Light, set to 60");
                light = 60;
            }
            dbQuery         = prop.getProperty("db.query");
            dbIp            = prop.getProperty("db.ip_postgres");
            try{dbPort = Integer.parseInt(prop.getProperty("db.port"));}
            catch(NumberFormatException ex)
            {
                log.warn("Cannot read dbPort, set to NULL");
                dbPort = null;
            }
            dbTable         = prop.getProperty("db.table");
            dbUser          = prop.getProperty("db.user");
            dbPsw           = prop.getProperty("db.psw");
            serverLocation  = prop.getProperty("server.location");            
            try{serverPort = Integer.parseInt(prop.getProperty("server.port"));}
            catch(NumberFormatException ex)
            {
                log.warn("Cannot read serverPort, set to NULL");
                serverPort = null;
            }
            
            printProperties();
            
        }
        catch(IOException ex)
        {
            log.error("Error reading params: ",ex.getStackTrace());
        }
        checkFileReadCorrectly=checkProperties();
    }

    private boolean checkProperties()
    {
        return (offset          != null &&
                light           != null &&
                dbQuery         != null &&
                dbIp            != null &&
                dbPort          != null &&
                dbTable         != null &&
                dbUser          != null &&
                dbPsw           != null &&
                serverLocation  != null &&
                serverPort      != null);
    }
    
    public double getOffset()
    {
        return offset;
    }

    public void setOffset(double offset)
    {
        this.offset = offset;
    }

    public Integer getLight()
    {
        return light;
    }

    public void setLight(Integer light)
    {
        this.light = light;
    }

    public String getDbQuery()
    {
        return dbQuery;
    }

    public void setDbQuery(String dbQuery)
    {
        this.dbQuery = dbQuery;
    }

    public String getDbIp()
    {
        return dbIp;
    }

    public void setDbIp(String dbIp)
    {
        this.dbIp = dbIp;
    }

    public Integer getDbPort()
    {
        return dbPort;
    }

    public void setDbPort(Integer dbPort)
    {
        this.dbPort = dbPort;
    }

    public String getDbTable()
    {
        return dbTable;
    }

    public void setDbTable(String dbTable)
    {
        this.dbTable = dbTable;
    }

    public String getDbUser()
    {
        return dbUser;
    }

    public void setDbUser(String dbUser)
    {
        this.dbUser = dbUser;
    }

    public String getDbPsw()
    {
        return dbPsw;
    }

    public void setDbPsw(String dbPsw)
    {
        this.dbPsw = dbPsw;
    }

    public String getServerLocation()
    {
        return serverLocation;
    }

    public void setServerLocation(String serverLocation)
    {
        this.serverLocation = serverLocation;
    }

    public Integer getServerPort()
    {
        return serverPort;
    }

    public void setServerPort(Integer serverPort)
    {
        this.serverPort = serverPort;
    }

    public boolean isReadCorrectly()
    {
        return checkFileReadCorrectly;
    }
    
    public void printProperties()
    {
        StringBuilder sb = new StringBuilder("Properties: ").append("\n");
        sb.append("dbQuery: ").append(dbQuery).append("\n");
        sb.append("dbPort: ").append(dbPort).append("\n");
        sb.append("dbTable: ").append(dbTable).append("\n");
        sb.append("dbUser: ").append(dbUser).append("\n");
        sb.append("serverLocation: ").append(serverLocation).append("\n");
        sb.append("serverPort: ").append(serverPort);
        
        log.info(sb.toString());
    }
}
