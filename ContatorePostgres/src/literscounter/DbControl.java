package literscounter;

import arlogg.ARLogg;
import java.sql.*;
import java.util.Map;

/**
 * @author Andrea Ravazzini
 * https://github.com/RavazziniAndrea
 */
public class DbControl
{
    static final ARLogg log = new ARLogg(DbControl.class, ARLogg.Level.INFO);
    
    public DbControl()
    {
    }
    
    public static double getQtyTotFromDb(Map<String,String> propertiesMap) throws SQLException
    {
        double qtyTot = -1;
        
        try(Connection c = getConnection(propertiesMap))
        {
            try(ResultSet rs = c.createStatement().executeQuery(propertiesMap.get("dbQuery")))
            {
                qtyTot = 0;
                while(rs.next())
                {
                    int qty = rs.getInt("qta");
                    String tipo = rs.getString("tipo");
                    
                    log.debug("qty: "+qty+" -- type: "+tipo);
                    switch(tipo.toLowerCase().split(" ")[0])
                    {
                        case "birra":
                            if(tipo.toLowerCase().contains("pong"))
                                qtyTot = qtyTot +(qty * LitersCounter.getLitersPong());
                            else    
                                qtyTot = qtyTot +(qty * LitersCounter.getLitersBeer());
                                
                            break;
                        case "drink":
                        case "amaro":
                            qtyTot = qtyTot +(qty * LitersCounter.getLitersShot());
                            break;
                        case "lambrusco":
                        case "rosso":
                        case "bianco":
                        case "prosecco":
                            qtyTot = qtyTot + (qty * LitersCounter.getLitersWineBottle());
                            break;
                        case "vodka":
                        case "gin":
                            qtyTot = qtyTot + (qty * LitersCounter.getLitersSuperBottle());
                            break;                        
                        case "bicchiere":
                            qtyTot = qtyTot + (qty * LitersCounter.getLitersGlass());
                            break;
                    }
                }                
            }
        }
        catch(SQLException ex)
        {
            log.error("ERROR QtyTot: " + ex.getLocalizedMessage());
            throw new SQLException();
        }
        
        return qtyTot;
    }
   
    public static Connection getConnection(Map<String, String> propertiesMap) throws SQLException
    {
        Connection c = DriverManager.getConnection("jdbc:postgresql://"+propertiesMap.get("dbIp")+":"+propertiesMap.get("dbPort")+"/"+ propertiesMap.get("dbTable") +"",propertiesMap.get("dbUser"),propertiesMap.get("dbPsw"));
        c.setAutoCommit(false);
        c.setReadOnly(true);
        c.setTransactionIsolation(Connection.TRANSACTION_READ_COMMITTED);
        return c;
    }
}
