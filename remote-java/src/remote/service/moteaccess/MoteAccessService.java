package remote.service.moteaccess;

import remote.service.Service;
import remote.service.ServiceCallback;

/** Service for acquiring mote usage privileges.
 *
 * This is a service interface for requesting mote usage privileges.
 * Example:
 * <pre>
 * MoteAccessService access = getMoteAccess();
 *
 * moteaccess.getMoteControl(session, motes, new ServiceCallback() {
 *	void onSuccess(Boolean[] privileged)
 *	{
 *		for (int i; i < privileged.length; i++) {
 *			if (privileged[i].booleanValue()) {
 *				// Do something.
 *			}
 *		}
 *	}
 *
 *	void onError(Throwable caught)
 *	{
 *		// Check error source.
 *	}
 * });
 * </pre>
 */
public interface MoteAccessService extends Service {

	/** Acquire mote control privileges using specific session.
	 *
	 * On success, the result object is a Boolean array
	 * with entries for each requested mote.
	 *
	 * @param session	ID of the session that wants privileges.
	 * @param motes		IDs of mote to control.
	 * @param callback	Callback for receiving result or error.
	 */
	void getMoteControl(String session, long[] motes,
		            ServiceCallback<Boolean[]> callback);

}
