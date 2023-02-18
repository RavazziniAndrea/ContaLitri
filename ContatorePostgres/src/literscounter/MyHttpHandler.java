package literscounter;

import com.sun.net.httpserver.HttpExchange;
import com.sun.net.httpserver.HttpHandler;
import java.io.IOException;
import java.io.OutputStream;

/**
 * @author Andrea Ravazzini
 * https://github.com/RavazziniAndrea
 */
public class MyHttpHandler implements HttpHandler
{
    static DataEsp32 dataEsp32;
    public MyHttpHandler(DataEsp32 datoEsp32)
    {
        MyHttpHandler.dataEsp32 = datoEsp32;
    }
    
    public static void updateData(DataEsp32 newEspData)
    {
        dataEsp32.setLiters(newEspData.getLiters());
        dataEsp32.setBrightness(newEspData.getBrightness());
    }
    
    @Override
    public void handle(HttpExchange t) throws IOException
    {
//        String response = "123#69";
        String response = ((int)dataEsp32.getLiters())+"#"+dataEsp32.getBrightness();
        
        t.sendResponseHeaders(200, response.length());
        try (OutputStream os = t.getResponseBody())
        {
            os.write(response.getBytes());
        }
    }
}
