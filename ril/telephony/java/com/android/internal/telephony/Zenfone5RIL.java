package com.android.internal.telephony;

import static com.android.internal.telephony.RILConstants.*;

import android.content.Context;
import android.media.AudioManager;
import android.os.AsyncResult;
import android.os.Message;
import android.os.Parcel;
import android.telephony.PhoneNumberUtils;
import android.telephony.Rlog;
import android.telephony.SignalStrength;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;

import com.android.internal.telephony.uicc.IccCardApplicationStatus;
import com.android.internal.telephony.uicc.IccCardStatus;

/**
 * Provides SignalStrength correction for old Zenfone5RIL extends RIL
 */
public class Zenfone5RIL extends RIL {

    public Zenfone5RIL(Context context, int NetworkType, int cdmaSubscription) {
        super(context, NetworkType, cdmaSubscription);
       mQANElements = 5;
    }

    public Zenfone5RIL(Context context, int NetworkType, int cdmaSubscription, Integer instanceId) {
        super(context, NetworkType, cdmaSubscription, instanceId);
       mQANElements = 5;
    }

    @Override
    protected Object
    responseSignalStrength(Parcel p) {
        int numInts = 12;
        int response[];

        response = new int[numInts];
        for (int i = 0 ; i < numInts ; i++) {
            if (i > 6 && i < 12) {
        //We dont have LTE so mark these as INVALID
                response[i] = SignalStrength.INVALID;
            } else {
                response[i] = p.readInt();
            }
        }

        return new SignalStrength(response[0], response[1], response[2], response[3], response[4], response[5],
                response[6], response[7],response[8], response[9], response[10], response[11], true);
    }

}