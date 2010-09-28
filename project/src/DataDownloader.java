// This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount
package com.googlecode.opentyrian;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import android.widget.TextView;
import org.apache.http.client.methods.*;
import org.apache.http.*;
import org.apache.http.impl.*;
import org.apache.http.impl.client.*;
import java.util.zip.*;
import java.io.*;
import android.util.Log;

import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;

class CountingInputStream extends BufferedInputStream {

	private long bytesReadMark = 0;
	private long bytesRead = 0;

	public CountingInputStream(InputStream in, int size) {

		super(in, size);
	}

	public CountingInputStream(InputStream in) {

		super(in);
	}

	public long getBytesRead() {

		return bytesRead;
	}

	public synchronized int read() throws IOException {

		int read = super.read();
		if (read >= 0) {
			bytesRead++;
		}
		return read;
	}

	public synchronized int read(byte[] b, int off, int len) throws IOException {

		int read = super.read(b, off, len);
		if (read >= 0) {
			bytesRead += read;
		}
		return read;
	}

	public synchronized long skip(long n) throws IOException {

		long skipped = super.skip(n);
		if (skipped >= 0) {
			bytesRead += skipped;
		}
		return skipped;
	}

	public synchronized void mark(int readlimit) {

		super.mark(readlimit);
		bytesReadMark = bytesRead;
	}

	public synchronized void reset() throws IOException {

		super.reset();
		bytesRead = bytesReadMark;
	}
}


class DataDownloader extends Thread
{
	class StatusWriter
	{
		private TextView Status;
		private MainActivity Parent;
		private String oldText = "";

		public StatusWriter( TextView _Status, MainActivity _Parent )
		{
			Status = _Status;
			Parent = _Parent;
		}
		public void setParent( TextView _Status, MainActivity _Parent )
		{
			synchronized(DataDownloader.this) {
				Status = _Status;
				Parent = _Parent;
				setText( oldText );
			}
		}
		
		public void setText(final String str)
		{
			class Callback implements Runnable
			{
				public TextView Status;
				public String text;
				public void run()
				{
					Status.setText(text + "\n" + Globals.ReadmeText);
				}
			}
			synchronized(DataDownloader.this) {
				Callback cb = new Callback();
				oldText = new String(str);
				cb.text = new String(str);
				cb.Status = Status;
				if( Parent != null && Status != null )
					Parent.runOnUiThread(cb);
			}
		}
		
	}
	public DataDownloader( MainActivity _Parent, TextView _Status )
	{
		Parent = _Parent;
		Status = new StatusWriter( _Status, _Parent );
		//Status.setText( "Connecting to " + Globals.DataDownloadUrl );
		outFilesDir = Parent.getFilesDir().getAbsolutePath();
		if( Globals.DownloadToSdcard )
			outFilesDir = "/sdcard/app-data/" + Globals.class.getPackage().getName();
		DownloadComplete = false;
		this.start();
	}
	
	public void setParent(MainActivity _Parent, TextView _Status)
	{
		synchronized(this) {
			Parent = _Parent;
			Status.setParent( _Status, _Parent );
		}
	}

	@Override
	public void run()
	{
		String [] downloadFiles = Globals.DataDownloadUrl.split("\\^");
		for( int i = 0; i < downloadFiles.length; i++ )
		{
			if( downloadFiles[i].length() > 0 && Globals.OptionalDataDownload.length > i && Globals.OptionalDataDownload[i] )
				if( ! DownloadDataFile(downloadFiles[i], "libsdl-DownloadFinished-" + String.valueOf(i) + ".flag") )
				{
					DownloadFailed = true;
					return;
				}
		}
		DownloadComplete = true;
		initParent();
	}

	public boolean DownloadDataFile(final String DataDownloadUrl, final String DownloadFlagFileName)
	{
		String [] downloadUrls = DataDownloadUrl.split("[|]");
		if( downloadUrls.length < 2 )
			return false;

		String path = getOutFilePath(DownloadFlagFileName);
		InputStream checkFile = null;
		try {
			checkFile = new FileInputStream( path );
		} catch( FileNotFoundException e ) {
		} catch( SecurityException e ) { };
		if( checkFile != null )
		{
			try {
				byte b[] = new byte[ Globals.DataDownloadUrl.getBytes("UTF-8").length + 1 ];
				int readed = checkFile.read(b);
				String compare = new String( b, 0, readed, "UTF-8" );
				boolean matched = false;
				System.out.println("Read URL: '" + compare + "'");
				for( int i = 1; i < downloadUrls.length; i++ )
				{
					System.out.println("Comparing: '" + downloadUrls[i] + "'");
					if( compare.compareTo(downloadUrls[i]) == 0 )
						matched = true;
				}
				System.out.println("Matched: " + String.valueOf(matched));
				if( ! matched )
					throw new IOException();
				Status.setText( "No need to download" );
				return true;
			} catch ( IOException e ) {};
		}
		checkFile = null;
		
		// Create output directory (not necessary for phone storage)
		if( Globals.DownloadToSdcard )
		{
			try {
				(new File( outFilesDir )).mkdirs();
			} catch( SecurityException e ) { };
		}

		HttpResponse response = null;
		HttpGet request;
		long totalLen;
		CountingInputStream stream;
		byte[] buf = new byte[16384];
		boolean DoNotUnzip = false;
		String url = "";

		int downloadUrlIndex = 1;
		while( downloadUrlIndex < downloadUrls.length ) 
		{
			System.out.println("Processing download " + downloadUrls[downloadUrlIndex]);
			url = new String(downloadUrls[downloadUrlIndex]);
			DoNotUnzip = false;
			if(url.indexOf(":") == 0)
			{
				url = url.substring( url.indexOf(":", 1) + 1 );
				DoNotUnzip = true;
			}
			System.out.println("Connecting to " + url);
			Status.setText( "Connecting to " + url);
			request = new HttpGet(url);
			request.addHeader("Accept", "*/*");
			try {
				DefaultHttpClient client = new DefaultHttpClient();
				client.getParams().setBooleanParameter("http.protocol.handle-redirects", true);
				response = client.execute(request);
			} catch (IOException e) {
				System.out.println("Failed to connect to " + downloadUrls[downloadUrlIndex]);
				downloadUrlIndex++;
			};
			if( response != null )
			{
				if( response.getStatusLine().getStatusCode() != 200 )
				{
					response = null;
					System.out.println("Failed to connect to " + url);
					downloadUrlIndex++;
				}
				else
					break;
			}
		}
		if( response == null )
		{
			System.out.println("Error connecting to " + url);
			Status.setText( "Error connecting to " + url );
			return false;
		}

		Status.setText( "Downloading data from " + url );
		totalLen = response.getEntity().getContentLength();
		try {
			stream = new CountingInputStream(response.getEntity().getContent());
		} catch( java.io.IOException e ) {
			Status.setText( "Error downloading data from " + Globals.DataDownloadUrl );
			return false;
		}
		
		if(DoNotUnzip)
		{
			path = getOutFilePath(downloadUrls[downloadUrlIndex].substring( 1,
					downloadUrls[downloadUrlIndex].indexOf(":", 1) ));
			OutputStream out = null;
			try {
				try {
					(new File( path.substring(0, path.lastIndexOf("/") ))).mkdirs();
				} catch( SecurityException e ) { };

				out = new FileOutputStream( path );
			} catch( FileNotFoundException e ) {
			} catch( SecurityException e ) { };
			if( out == null )
			{
				Status.setText( "Error writing to " + path );
				return false;
			}

			try {
				int len = stream.read(buf);
				while (len >= 0)
				{
					if(len > 0)
						out.write(buf, 0, len);
					len = stream.read(buf);

					String percent = "";
					if( totalLen > 0 )
						percent = String.valueOf(stream.getBytesRead() * 100 / totalLen) + "%: ";
					Status.setText( percent + "writing file " + path );
				}
				out.flush();
				out.close();
				out = null;
			} catch( java.io.IOException e ) {
				Status.setText( "Error writing file " + path + " from URL " + url );
				return false;
			}
		}
		else
		{
			ZipInputStream zip = new ZipInputStream(stream);
			
			while(true)
			{
				ZipEntry entry = null;
				try {
					entry = zip.getNextEntry();
				} catch( java.io.IOException e ) {
					Status.setText( "Error downloading data from " + url );
					return false;
				}
				if( entry == null )
					break;
				if( entry.isDirectory() )
				{
					try {
						(new File( getOutFilePath(entry.getName()) )).mkdirs();
					} catch( SecurityException e ) { };
					continue;
				}

				OutputStream out = null;
				path = getOutFilePath(entry.getName());
				
				try {
					CheckedInputStream check = new CheckedInputStream( new FileInputStream(path), new CRC32() );
					while( check.read(buf, 0, buf.length) > 0 ) {};
					check.close();
					if( check.getChecksum().getValue() != entry.getCrc() )
					{
						File ff = new File(path);
						ff.delete();
						throw new Exception();
					}
					continue;
				} catch( Exception e )
				{
				}

				try {
					out = new FileOutputStream( path );
				} catch( FileNotFoundException e ) {
				} catch( SecurityException e ) { };
				if( out == null )
				{
					Status.setText( "Error writing to " + path );
					return false;
				}

				String percent = "";
				if( totalLen > 0 )
					percent = String.valueOf(stream.getBytesRead() * 100 / totalLen) + "%: ";
				Status.setText( percent + "writing file " + path );
				
				try {
					int len = zip.read(buf);
					while (len >= 0)
					{
						if(len > 0)
							out.write(buf, 0, len);
						len = zip.read(buf);

						percent = "";
						if( totalLen > 0 )
							percent = String.valueOf(stream.getBytesRead() * 100 / totalLen) + "%: ";
						Status.setText( percent + "writing file " + path );
					}
					out.flush();
					out.close();
					out = null;
				} catch( java.io.IOException e ) {
					Status.setText( "Error writing file " + path + " from URL " + url );
					return false;
				}
				
				try {
					CheckedInputStream check = new CheckedInputStream( new FileInputStream(path), new CRC32() );
					while( check.read(buf, 0, buf.length) > 0 ) {};
					check.close();
					if( check.getChecksum().getValue() != entry.getCrc() )
					{
						File ff = new File(path);
						ff.delete();
						throw new Exception();
					}
				} catch( Exception e )
				{
					Status.setText( "CRC32 check failed for file " + path );
					return false;
				}
			}
		};

		OutputStream out = null;
		path = getOutFilePath(DownloadFlagFileName);
		try {
			out = new FileOutputStream( path );
			out.write(downloadUrls[downloadUrlIndex].getBytes("UTF-8"));
			out.flush();
			out.close();
		} catch( FileNotFoundException e ) {
		} catch( SecurityException e ) {
		} catch( java.io.IOException e ) {
			Status.setText( "Error writing file " + path );
			return false;
		};
		Status.setText( "Finished" );

		try {
			stream.close();
		} catch( java.io.IOException e ) {
		};

		return true;
	};
	
	private void initParent()
	{
		class Callback implements Runnable
		{
			public MainActivity Parent;
			public void run()
			{
				Parent.initSDL();
			}
		}
		Callback cb = new Callback();
		synchronized(this) {
			cb.Parent = Parent;
			if(Parent != null)
				Parent.runOnUiThread(cb);
		}
	}
	
	private String getOutFilePath(final String filename)
	{
		return outFilesDir + "/" + filename;
	};
	
	public StatusWriter Status;
	public boolean DownloadComplete = false;
	public boolean DownloadFailed = false;
	private MainActivity Parent;
	private String outFilesDir = null;
}

