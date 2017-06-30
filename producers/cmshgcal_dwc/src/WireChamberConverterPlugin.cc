#include "eudaq/DataConverterPlugin.hh"
#include "eudaq/StandardEvent.hh"
#include "eudaq/Utils.hh"
#include "eudaq/Logger.hh"

#include <bitset>
#include <boost/format.hpp>

// All LCIO-specific parts are put in conditional compilation blocks
// so that the other parts may still be used if LCIO is not available.
#if USE_LCIO
#include "IMPL/LCEventImpl.h"
#include "IMPL/TrackerRawDataImpl.h"
#include "IMPL/LCCollectionVec.h"
#include "lcio.h"
#endif

namespace eudaq {

  // The event type for which this converter plugin will be registered
  // Modify this to match your actual event type (from the Producer)
  //static const char *EVENT_TYPE = "RPI";
  static const char *EVENT_TYPE = "WireChambers";

  // Declare a new class that inherits from DataConverterPlugin
  class WireChamberConverterPlugin : public DataConverterPlugin {

  public:
    // This is called once at the beginning of each run.
    // You may extract information from the BORE and/or configuration
    // and store it in member variables to use during the decoding later.
    virtual void Initialize(const Event &bore, const Configuration &cnf) {

#ifndef WIN32 // some linux Stuff //$$change
      (void)cnf; // just to suppress a warning about unused parameter cnf
#endif
    }

    // This should return the trigger ID (as provided by the TLU)
    // if it was read out, otherwise it can either return (unsigned)-1,
    // or be left undefined as there is already a default version.
    virtual unsigned GetTriggerID(const Event &ev) const {
      static const unsigned TRIGGER_OFFSET = 8;
      // Make sure the event is of class RawDataEvent
      if (const RawDataEvent *rev = dynamic_cast<const RawDataEvent *>(&ev)) {
	// This is just an example, modified it to suit your raw data format
        // Make sure we have at least one block of data, and it is large enough
        if (rev->NumBlocks() > 0 &&
            rev->GetBlock(0).size() >= (TRIGGER_OFFSET + sizeof(short))) {
          // Read a little-endian unsigned short from offset TRIGGER_OFFSET
          return getlittleendian<unsigned short>( &rev->GetBlock(0)[TRIGGER_OFFSET]);
        }
      }
      // If we are unable to extract the Trigger ID, signal with (unsigned)-1
      return (unsigned)-1;
    }

    // Here, the data from the RawDataEvent is extracted into a StandardEvent.
    // The return value indicates whether the conversion was successful.
    // Again, this is just an example, adapted it for the actual data layout.
    virtual bool GetStandardSubEvent(StandardEvent &sev, const Event &ev) const {

      // If the event type is used for different sensors
      // they can be differentiated here
      const std::string sensortype = "WireChamber";

      std::cout<<"\t Dans GetStandardSubEvent()  "<<std::endl;

      const RawDataEvent * rev = dynamic_cast<const RawDataEvent *> ( &ev );

      //rev->Print(std::cout);

      const unsigned nBlocks = rev->NumBlocks();
      std::cout<<"Number of Raw Data Blocks: "<<nBlocks<<std::endl;

      for (unsigned blo=0; blo<nBlocks; blo++){

              
        std::cout<<"Block = "<<blo<<"  Raw GetID = "<<rev->GetID(blo)<<std::endl;

        const RawDataEvent::data_t & bl = rev->GetBlock(blo);

        std::cout<<"size of block: "<<bl.size()<<std::endl;

        float x1,x2,y1,y2,x3,y3,x4,y4;  
        memcpy(&x1, &bl[0], sizeof(x1));
        memcpy(&y1, &bl[4], sizeof(y1));
        memcpy(&x2, &bl[8], sizeof(x2));
        memcpy(&y2, &bl[12], sizeof(y2));
        memcpy(&x3, &bl[16], sizeof(x3));
        memcpy(&y3, &bl[20], sizeof(y3));
        memcpy(&x4, &bl[24], sizeof(x4));
        memcpy(&y4, &bl[28], sizeof(y4));

        std::cout<<"x1="<<x1<<" y1="<<y1<<"     x2="<<x2<<" y2="<<y2;
        std::cout<<" x3="<<x3<<" y3="<<y3<<"     x4="<<x4<<" y4="<<y4<<std::endl;

        
        StandardPlane wc1(0, EVENT_TYPE, sensortype);
        StandardPlane wc2(1, EVENT_TYPE, sensortype);
        StandardPlane wc3(2, EVENT_TYPE, sensortype);
        StandardPlane wc4(3, EVENT_TYPE, sensortype);
        wc1.SetSizeRaw(1, 2);
        wc2.SetSizeRaw(1, 2);
        wc3.SetSizeRaw(1, 2);
        wc4.SetSizeRaw(1, 2);
        // We store 4 numbers into these "planes":
        // X1,Y1 for first WC, X2,Y2 for second WC  
        wc1.SetPixel(0, 0, 0, x1);
        wc1.SetPixel(1, 0, 1, y1);
        
        wc2.SetPixel(0, 0, 0, x2);
        wc2.SetPixel(1, 0, 1, y2);

        wc3.SetPixel(0, 0, 0, x3);
        wc3.SetPixel(1, 0, 1, y3);

        wc4.SetPixel(0, 0, 0, x4);
        wc4.SetPixel(1, 0, 1, y4);
        
        // Set the trigger ID
        wc1.SetTLUEvent(GetTriggerID(ev));
        wc2.SetTLUEvent(GetTriggerID(ev));
        wc3.SetTLUEvent(GetTriggerID(ev));
        wc4.SetTLUEvent(GetTriggerID(ev));
        
        // Add the plane to the StandardEvent
        sev.AddPlane(wc1);
        sev.AddPlane(wc2);
        sev.AddPlane(wc3);
        sev.AddPlane(wc4);

      	eudaq::mSleep(10);
	
      }
      

      
      std::cout<<"St Ev NumPlanes: "<<sev.NumPlanes()<<std::endl;

      // Indicate that data was successfully converted
      return true;

    }


#if USE_LCIO
    // This is where the conversion to LCIO is done
    virtual lcio::LCEvent *GetLCIOEvent(const Event * /*ev*/) const {
      return 0;
    }
#endif

  private:
    // The constructor can be private, only one static instance is created
    // The DataConverterPlugin constructor must be passed the event type
    // in order to register this converter for the corresponding conversions
    // Member variables should also be initialized to default values here.
    WireChamberConverterPlugin()
        : DataConverterPlugin(EVENT_TYPE) {}

    // Information extracted in Initialize() can be stored here:


    // The single instance of this converter plugin
    static WireChamberConverterPlugin m_instance;
    }; // class WireChamberConverterPlugin

    // Instantiate the converter plugin instance
    WireChamberConverterPlugin WireChamberConverterPlugin::m_instance;

  } // namespace eudaq
