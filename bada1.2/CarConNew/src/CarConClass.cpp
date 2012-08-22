/*
 Copyright (C) 2012  Boštjan Mrak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
 * CarConClass.cpp
 *
 *  Created on: 16.8.2010
 *      Author: mrak
 */

#include "CarConClass.h"

using namespace Osp::App;
using namespace Osp::Base;
using namespace Osp::System;
using namespace Osp::Locales;

Osp::Base::String CarConClass::DateToSQLlite(Osp::Base::DateTime date) {
	String tmps = L"";
	tmps.Format(20,L"%04dD%02dD%02d %02dT%02dT%02d",date.GetYear(),date.GetMonth(),date.GetDay(),date.GetHour(),date.GetMinute(),date.GetSecond());
	tmps.Replace(L"D",L"-");
	tmps.Replace(L"T",L":");
	return tmps;
}

Osp::Base::String CarConClass::DateToSQLlite(const DbStatement &pSt_, int colindex, Osp::Base::DateTime date) {
	DbStatement *pSts_ = (DbStatement*) &pSt_;
	String tmps;
	tmps = this->DateToSQLlite(date);
	pSts_->BindString(colindex, tmps);
	return tmps;
}

Osp::Base::DateTime CarConClass::DateFromSQLlite(Osp::Base::String date) {
	String tmps;
	int year, month, day, hour, min, sec;
	Osp::Base::DateTime result;
	tmps = L"";
	date.SubString(0,4,tmps);
	Osp::Base::Integer::Parse(tmps,10,year);
	date.SubString(5,2,tmps);
	Osp::Base::Integer::Parse(tmps,10,month);
	date.SubString(8,2,tmps);
	Osp::Base::Integer::Parse(tmps,10,day);
	date.SubString(11,2,tmps);
	Osp::Base::Integer::Parse(tmps,10,hour);
	date.SubString(14,2,tmps);
	Osp::Base::Integer::Parse(tmps,10,min);
	date.SubString(17,2,tmps);
	Osp::Base::Integer::Parse(tmps,10,sec);
	result.SetValue(year,month,day,hour,min,sec);
	return result;
}

Osp::Base::DateTime CarConClass::DateFromSQLlite(const DbEnumerator & pEn_, int colindex, Osp::Base::DateTime & result) {
	DbEnumerator *pEns_ = (DbEnumerator*) &pEn_;
	String tmps;
	pEns_->GetStringAt(colindex, tmps);
	result = this->DateFromSQLlite(tmps);
	return result;
}

void CarConClass::CloseDatabase() {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pEnum_ = null;
	if (db_ != null)
	delete db_;
	db_ = null;
}

CarConClass::CarConClass() {
	this->SelectedCar.Brand = "";
	this->SelectedCar.Model = "";
	this->SelectedCar.ID = 0;
	this->db_ = null;
	this->pEnum_ = null;
	this->pStmt_ = null;
	this->isfiststart = false;
}
int CarConClass::GetLastSelectedID(void) {
	int intvalue;
	String value;
	result r;
	r = Application::GetInstance()->GetAppRegistry()->Get(L"selectedcarid", value);
	if ( r == E_KEY_NOT_FOUND) {
		intvalue = -1;
	} else {
		r = Integer::Parse(value, intvalue);
		if (IsFailed(r)) {
			intvalue = 0;
		}
	}
	return intvalue;
}
bool CarConClass::SetLastSelectedID(int lastselectedid) {
	String value;
	result r;
	GetCarData(lastselectedid, this->SelectedCar);
	r = Application::GetInstance()->GetAppRegistry()->Get(L"selectedcarid", value);
	if ( r == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"selectedcarid", lastselectedid);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"selectedcarid", lastselectedid);
	}
	Application::GetInstance()->GetAppRegistry()->Save();
	return true;
}
bool CarConClass::GetCarData(int carid, CarData & data, bool readfueltypes) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	if (readfueltypes == true) {
	data.fueltypesarray.Construct(0);
	data.fueltypesstring = "";
	}
	data.ID = 0;
	data.Brand = L"";
	data.Model = L"";
	data.fueltype = 0;
	data.startkm = 0;
	data.curodometervalue = 0;
	data.isurance = L"";
	data.licence = L"";
	data.time = data.time.GetMinValue();
	data.vin = L"";
	data.year = 0;
	String sql;
	sql = L"SELECT ID, BRAND, MODEL, FUELTYPE, STARTKM, TIME, YEAR, LICENCE, VIN, INSURANCE FROM CARS WHERE ID=";
	sql.Append(carid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetStringAt(1, data.Brand);
		pEnum_->GetStringAt(2, data.Model);
		pEnum_->GetIntAt(3, data.fueltype);
		pEnum_->GetIntAt(4, data.startkm);
		data.curodometervalue = data.startkm;
		CarConData condata_;
		this->GetCarConGetLastData(carid, 0, condata_);
		if (condata_.StKm > data.curodometervalue)
		data.curodometervalue = condata_.StKm;
		this->DateFromSQLlite(*pEnum_,5, data.time);
		pEnum_->GetIntAt(6, data.year);
		if (pEnum_->GetColumnSize(7) > 0) {
		pEnum_->GetStringAt(7, data.licence);
		} else {
		data.licence = L"";
		}
		if (pEnum_->GetColumnSize(8) > 0) {
		pEnum_->GetStringAt(8, data.vin);
		} else {
		data.vin = L"";
		}
		if (pEnum_->GetColumnSize(9) > 0) {
		pEnum_->GetStringAt(9, data.isurance);
		} else {
		data.isurance = L"";
		}
		if (readfueltypes == true) {
		if (pEnum_ != null)
		delete pEnum_;
		pEnum_ = null;
		sql = L"SELECT CARSFUELTYPES.ID, CARID, FUELTYPEID, FUELTYPES.CAPTION FROM CARSFUELTYPES INNER JOIN FUELTYPES ON FUELTYPES.ID = CARSFUELTYPES.FUELTYPEID WHERE CARSFUELTYPES.CARID=";
		sql.Append(carid);
		pEnum_ = db_->QueryN(sql);
		if(!(!pEnum_)) {
			pEnum_->Reset();
			while (pEnum_->MoveNext() ==  E_SUCCESS) {
				int fueltypeid;
				String fueltypestr;
				pEnum_->GetStringAt(3, fueltypestr);
				pEnum_->GetIntAt(2, fueltypeid);
				data.fueltypesarray.Add(fueltypeid);
				if (data.fueltypesstring != "")
					data.fueltypesstring += ", ";
				data.fueltypesstring += fueltypestr;
			}
		}
		}
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveCarData(CarData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE CARS SET BRAND = ?, MODEL = ?, FUELTYPE = ?, YEAR = ?, LICENCE = ?, VIN = ?, INSURANCE = ? WHERE ID=?");
		pStmt_->BindString(0, data.Brand);
		pStmt_->BindString(1, data.Model);
		pStmt_->BindInt(2, data.fueltype);
		pStmt_->BindInt(3, data.year);
		pStmt_->BindString(4, data.licence);
		pStmt_->BindString(5, data.vin);
		pStmt_->BindString(6, data.isurance);
		pStmt_->BindInt(7, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO CARS (ID, BRAND, MODEL, FUELTYPE, STARTKM, TIME, YEAR, LICENCE, VIN, INSURANCE) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pStmt_->BindString(0, data.Brand);
		pStmt_->BindString(1, data.Model);
		pStmt_->BindInt(2, data.fueltype);
		pStmt_->BindInt(3, data.startkm);
		DateTime today;
		Osp::System::SystemTime::GetCurrentTime(STANDARD_TIME, today);
		this->DateToSQLlite(*pStmt_, 4, today);
		pStmt_->BindInt(5, data.year);
		pStmt_->BindString(6, data.licence);
		pStmt_->BindString(7, data.vin);
		pStmt_->BindString(8, data.isurance);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	String sql;
	sql = L"DELETE FROM CARSFUELTYPES WHERE CARID=";
	sql.Append(data.ID);
	db_->ExecuteSql(sql, true);
	IEnumeratorT<int>* enumint = data.fueltypesarray.GetEnumeratorN();
	if (enumint){
		if (pStmt_ != null)
		delete pStmt_;
		pStmt_ = null;
		int fueltypeid;
		while (enumint->MoveNext() == E_SUCCESS){
			db_->BeginTransaction();
			enumint->GetCurrent(fueltypeid);
			pStmt_ = db_->CreateStatementN(L"INSERT INTO CARSFUELTYPES (ID, CARID, FUELTYPEID) VALUES (NULL, ?, ?)");
			pStmt_->BindInt(0, data.ID);
			pStmt_->BindInt(1, fueltypeid);
			db_->ExecuteStatementN(*pStmt_);
			db_->CommitTransaction();
			if (pStmt_ != null)
			delete pStmt_;
			pStmt_ = null;
		}
		delete enumint;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;

	if ((this->SelectedCar.ID == data.ID) && (data.ID > 0)) {
		if (!this->GetCarData(data.ID, this->SelectedCar)) {
			this->SelectedCar.ID = 0;
		}
	}

	return true;
}
bool CarConClass::DeleteCarData(int carid) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	sql = L"DELETE FROM CON WHERE CARID=";
	sql.Append(carid);
	db_->ExecuteSql(sql, true);
	sql = L"DELETE FROM CARSFUELTYPES WHERE CARID=";
	sql.Append(carid);
	db_->ExecuteSql(sql, true);
	sql = L"DELETE FROM EXPENSES WHERE CARID=";
	sql.Append(carid);
	db_->ExecuteSql(sql, true);
	sql = L"DELETE FROM CARS WHERE ID=";
	sql.Append(carid);
	db_->ExecuteSql(sql, true);

	if (this->SelectedCar.ID == carid) {
		this->SelectedCar.ID = 0;
	}
	return true;
}
bool CarConClass::GetCarDataListStart(void) {
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT ID, BRAND, MODEL FROM CARS ORDER BY ID");
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetCarDataListGetData(CarData & data) {
	int intvalue;
	String value;
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetStringAt(1, value);
		data.Brand = value;
		pEnum_->GetStringAt(2, value);
		data.Model = value;
		pEnum_->GetIntAt(0, intvalue);
		data.ID = intvalue;
		pEnum_->GetIntAt(6, data.year);
		if (pEnum_->GetColumnSize(7) > 0) {
		pEnum_->GetStringAt(7, data.licence);
		} else {
		data.licence = L"";
		}
		if (pEnum_->GetColumnSize(8) > 0) {
		pEnum_->GetStringAt(8, data.vin);
		} else {
		data.vin = L"";
		}
		if (pEnum_->GetColumnSize(9) > 0) {
		pEnum_->GetStringAt(9, data.isurance);
		} else {
		data.isurance = L"";
		}
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetCarDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetFuelTypeDataListStart(int filtercarid, bool reversefilter) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	sql = L"SELECT FUELTYPES.ID, FUELTYPES.CAPTION FROM FUELTYPES";
	if (filtercarid > 0) {
		sql.Append(L" LEFT JOIN CARSFUELTYPES ON CARSFUELTYPES.FUELTYPEID = FUELTYPES.ID");
		if (reversefilter == true) {
			sql.Append(L" WHERE CARSFUELTYPES.CARID IS NULL OR CARSFUELTYPES.CARID !=");
		} else {
			sql.Append(L" WHERE CARSFUELTYPES.CARID=");
		}
		sql.Append(filtercarid);
	}
	sql.Append(" ORDER BY FUELTYPES.ID");
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetFuelTypeDataListGetData(FuelTypeData & data) {
	int intvalue;
	String value;
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetStringAt(1, value);
		data.Caption = value;
		pEnum_->GetIntAt(0, intvalue);
		data.ID = intvalue;
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetFuelTypeDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetFuelTypeData(int itemid, FuelTypeData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT ID, CAPTION FROM FUELTYPES WHERE ID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetStringAt(1, data.Caption);
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveFuelTypeData(FuelTypeData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE FUELTYPES SET CAPTION = ? WHERE ID=?");
		pStmt_->BindString(0, data.Caption);
		pStmt_->BindInt(1, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO FUELTYPES (ID, CAPTION) VALUES (NULL, ?)");
		pStmt_->BindString(0, data.Caption);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
bool CarConClass::DeleteFuelTypeData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	bool candelete = true;
	sql = L"SELECT ID, CARID, FUELTYPEID FROM CARSFUELTYPES WHERE FUELTYPEID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!(!pEnum_)) {
		candelete = false;
	}
	if (candelete == true) {
	sql = L"SELECT ID, FUELTYPE FROM CON WHERE FUELTYPE=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!(!pEnum_)) {
		candelete = false;
	}
	}
	if (candelete == true) {
		if (justcheck != true) {
			sql = L"DELETE FROM CARSFUELTYPES WHERE FUELTYPEID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
			sql = L"DELETE FROM FUELTYPES WHERE ID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
		}
		returnvalue = true;
	} else {
		returnvalue = false;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::GetLocationDataListStart(void) {
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT ID, CAPTION FROM LOCATIONS ORDER BY CAPTION, ID");
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetLocationDataListStart(Osp::Base::String filtercaption, int limit) {
	if (db_ == null) {
		this->Create(0);
	}
	if (filtercaption.GetLength() > 0) {
		if (limit > 0) {
			pStmt_ = db_->CreateStatementN(L"SELECT ID, CAPTION FROM LOCATIONS WHERE (CAPTION LIKE ?) OR (CAPTION LIKE ?) ORDER BY CAPTION LIMIT " + Osp::Base::Integer::ToString(limit));
		} else {
			pStmt_ = db_->CreateStatementN(L"SELECT ID, CAPTION FROM LOCATIONS WHERE (CAPTION LIKE ?) OR (CAPTION LIKE ?) ORDER BY CAPTION");
		}
		pStmt_->BindString(0, filtercaption + "%");
		pStmt_->BindString(1, "% " + filtercaption + "%");
	} else {
		if (limit > 0) {
			pStmt_ = db_->CreateStatementN(L"SELECT ID, CAPTION FROM LOCATIONS ORDER BY CAPTION LIMIT " + Osp::Base::Integer::ToString(limit));
		} else {
			pStmt_ = db_->CreateStatementN(L"SELECT ID, CAPTION FROM LOCATIONS ORDER BY CAPTION");
		}
	}
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetLocationDataListGetData(LocationData & data) {
	int intvalue;
	String value;
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetStringAt(1, value);
		data.Caption = value;
		pEnum_->GetIntAt(0, intvalue);
		data.ID = intvalue;
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetLocationDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetLocationData(int itemid, LocationData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT ID, CAPTION FROM LOCATIONS WHERE ID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetStringAt(1, data.Caption);
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveLocationData(LocationData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE LOCATIONS SET CAPTION = ? WHERE ID=?");
		pStmt_->BindString(0, data.Caption);
		pStmt_->BindInt(1, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO LOCATIONS (ID, CAPTION) VALUES (NULL, ?)");
		pStmt_->BindString(0, data.Caption);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}

bool CarConClass::DeleteLocationData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	bool candelete = true;
	sql = L"SELECT ID, LOCATION FROM CON WHERE LOCATION=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!(!pEnum_)) {
		candelete = false;
	}
	if (candelete == true) {
	sql = L"SELECT ID, STARTLOCATION, ENDLOCATION FROM TRIP WHERE (STARTLOCATION=";
	sql.Append(itemid);
	sql.Append(L") OR (ENDLOCATION=");
	sql.Append(itemid);
	sql.Append(L")");
	pEnum_ = db_->QueryN(sql);
	if(!(!pEnum_)) {
		candelete = false;
	}
	}
	if (candelete == true) {
		if (justcheck != true) {
			sql = L"DELETE FROM LOCATIONS WHERE ID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
		}
		returnvalue = true;
	} else {
		returnvalue = false;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
int CarConClass::SelectAddLocationData(Osp::Base::String caption) {
	int foundid(0);
	pStmt_ = db_->CreateStatementN(L"SELECT ID, CAPTION FROM LOCATIONS WHERE CAPTION LIKE ? ORDER BY CAPTION");
	pStmt_->BindString(0, caption);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if (!(!pEnum_)) {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, foundid);
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	if (foundid == 0) {
		LocationData data_;
		data_.Caption = caption;
		data_.ID = 0;
		if (SaveLocationData(data_)) {
			foundid = data_.ID;
		}
	}
	return foundid;
}

bool CarConClass::GetExpenseKatDataListStart(void) {
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT ID, CAPTION FROM EXPENSESKAT ORDER BY ID");
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetExpenseKatDataListGetData(CarExpenseKatData & data) {
	int intvalue;
	String value;
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetStringAt(1, value);
		data.Caption = value;
		pEnum_->GetIntAt(0, intvalue);
		data.ID = intvalue;
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetExpenseKatDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetExpenseKatData(int itemid, CarExpenseKatData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT ID, CAPTION FROM EXPENSESKAT WHERE ID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetStringAt(1, data.Caption);
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveExpenseKatData(CarExpenseKatData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE EXPENSESKAT SET CAPTION = ? WHERE ID=?");
		pStmt_->BindString(0, data.Caption);
		pStmt_->BindInt(1, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO EXPENSESKAT (ID, CAPTION) VALUES (NULL, ?)");
		pStmt_->BindString(0, data.Caption);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
bool CarConClass::DeleteExpenseKatData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	bool candelete = true;
	sql = L"SELECT ID, KATID FROM EXPENSES WHERE KATID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!(!pEnum_)) {
		candelete = false;
	}
	if (candelete == true) {
		if (justcheck != true) {
			sql = L"DELETE FROM EXPENSESKAT WHERE ID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
		}
		returnvalue = true;
	} else {
		returnvalue = false;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::GetExpenseDataFilterListStart(int carid, DateTime filterfromdate, DateTime filtertodate) {
	if (db_ == null) {
		this->Create(0);
	}
	pStmt_ = db_->CreateStatementN(L"SELECT EXPENSES.ID, CARID, EXPENSES.CAPTION, TIME, PRICE, REMARK, ODOMETER, KATID, EXPENSESKAT.CAPTION AS KATCAPTION FROM EXPENSES LEFT JOIN EXPENSESKAT ON EXPENSESKAT.ID = EXPENSES.KATID WHERE CARID=? AND TIME>=? AND TIME<=? ORDER BY TIME DESC, EXPENSES.ID DESC");
	pStmt_->BindInt(0, carid);
	this->DateToSQLlite(*pStmt_, 1, filterfromdate);
	this->DateToSQLlite(*pStmt_, 2, filtertodate);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetExpenseDataListStart(int carid) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	sql = L"SELECT EXPENSES.ID, CARID, EXPENSES.CAPTION, TIME, PRICE, REMARK, ODOMETER, KATID, EXPENSESKAT.CAPTION AS KATCAPTION FROM EXPENSES LEFT JOIN EXPENSESKAT ON EXPENSESKAT.ID = EXPENSES.KATID WHERE CARID=";
	sql.Append(carid);
	sql.Append(L" ORDER BY TIME DESC, EXPENSES.ID DESC");
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetExpenseDataListGetData(CarExpenseData & data) {
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		pEnum_->GetStringAt(2, data.Caption);
		this->DateFromSQLlite(*pEnum_,3, data.time);
		pEnum_->GetDoubleAt(4, data.Price);
		if (pEnum_->GetColumnSize(5) > 0) {
		pEnum_->GetStringAt(5, data.Remark);
		} else {
		data.Remark = L"";
		}
		pEnum_->GetIntAt(6, data.Odometer);
		pEnum_->GetIntAt(7, data.katid);
		if (pEnum_->GetColumnSize(8) > 0) {
		pEnum_->GetStringAt(8, data.katcaption);
		} else {
		data.katcaption = L"";
		}
		data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetExpenseDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
bool CarConClass::GetExpenseData(int itemid, CarExpenseData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT EXPENSES.ID, CARID, EXPENSES.CAPTION, TIME, PRICE, REMARK, ODOMETER, KATID, EXPENSESKAT.CAPTION AS KATCAPTION FROM EXPENSES LEFT JOIN EXPENSESKAT ON EXPENSESKAT.ID = EXPENSES.KATID WHERE EXPENSES.ID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		data.CarID = this->SelectedCar.ID;
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		pEnum_->GetStringAt(2, data.Caption);
		this->DateFromSQLlite(*pEnum_,3, data.time);
		pEnum_->GetDoubleAt(4, data.Price);
		if (pEnum_->GetColumnSize(5) > 0) {
		pEnum_->GetStringAt(5, data.Remark);
		} else {
		data.Remark = L"";
		}
		pEnum_->GetIntAt(6, data.Odometer);
		pEnum_->GetIntAt(7, data.katid);
		if (pEnum_->GetColumnSize(8) > 0) {
		pEnum_->GetStringAt(8, data.katcaption);
		} else {
		data.katcaption = L"";
		}
		data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveExpenseData(CarExpenseData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE EXPENSES SET CAPTION = ?, TIME = ?, PRICE = ?, REMARK = ?, ODOMETER = ?, KATID = ? WHERE ID=?");
		pStmt_->BindString(0, data.Caption);
		this->DateToSQLlite(*pStmt_, 1, data.time);
		pStmt_->BindDouble(2, data.Price);
		pStmt_->BindString(3, data.Remark);
		pStmt_->BindInt(4, data.Odometer);
		pStmt_->BindInt(5, data.katid);
		pStmt_->BindInt(6, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO EXPENSES (ID, CARID, CAPTION, TIME, PRICE, REMARK, ODOMETER, KATID) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?)");
		pStmt_->BindInt(0, data.CarID);
		pStmt_->BindString(1, data.Caption);
		this->DateToSQLlite(*pStmt_, 2, data.time);
		pStmt_->BindDouble(3, data.Price);
		pStmt_->BindString(4, data.Remark);
		pStmt_->BindInt(5, data.Odometer);
		pStmt_->BindInt(6, data.katid);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
bool CarConClass::DeleteExpenseData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	bool candelete = true;
	if (candelete == true) {
		if (justcheck != true) {
			sql = L"DELETE FROM EXPENSES WHERE ID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
		}
		return true;
	} else {
		return false;
	}
}
bool CarConClass::Create(int lastselectedid) {
	if (Osp::Io::File::IsFileExist(L"/Home/temprestore.db")) {
		Osp::Io::File::Remove(L"/Home/temprestore.db");
	}
	if (db_ == null) {
		db_ = new Database();
		if (!Database::Exists(L"/Home/data.db")) {
			db_->Construct((L"/Home/data.db"), true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS CARS (ID INTEGER PRIMARY KEY AUTOINCREMENT, BRAND TEXT, MODEL TEXT, FUELTYPE INTEGER, STARTKM BIGINT, STARTCON DOUBLE, TIME DATETIME, YEAR INTEGER, LICENCE TEXT, VIN TEXT, INSURANCE TEXT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS CARSFUELTYPES (ID INTEGER PRIMARY KEY AUTOINCREMENT, CARID INTEGER, FUELTYPEID INTEGER)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS FUELTYPES (ID INTEGER PRIMARY KEY AUTOINCREMENT, CAPTION TEXT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS LOCATIONS (ID INTEGER PRIMARY KEY AUTOINCREMENT, CAPTION TEXT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS CON (ID INTEGER PRIMARY KEY AUTOINCREMENT, CARID INTEGER, FUELTYPE INTEGER, LOCATION INTEGER, TYPE INTEGER, TIME DATETIME, STKM BIGINT, STL DOUBLE, PRICE DOUBLE, REMARK TEXT, CON DOUBLE, TRIP BIGINT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS EXPENSES (ID INTEGER PRIMARY KEY AUTOINCREMENT, CARID INTEGER, CAPTION TEXT, TIME DATETIME, PRICE DOUBLE, REMARK TEXT, ODOMETER BIGINT, KATID INTEGER)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS EXPENSESKAT (ID INTEGER PRIMARY KEY AUTOINCREMENT, CAPTION TEXT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS SETTINGS (ID INTEGER PRIMARY KEY, DBVER INTEGER, DISTANCEUNIT INTEGER, VOLUMEUNIT INTEGER, CONUNIT INTEGER, CURRENCY TEXT, AVGCALC INTEGER)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS TRIP (ID INTEGER PRIMARY KEY AUTOINCREMENT, CARID INTEGER, STOSEB INTEGER, STARTTIME DATETIME, STARTODOMETER INTEGER, STARTLOCATION INTEGER, ENDTIME DATETIME, ENDODOMETER INTEGER, ENDLOCATION INTEGER, PRICEPERL DOUBLE, CALCEXPENSES DOUBLE, REALEXPENSES DOUBLE, REMARK TEXT)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS PARKINGPRICELIST (ID INTEGER PRIMARY KEY AUTOINCREMENT, CAPTION TEXT, LOCATION INTEGER, UPDATETIME DATETIME)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS PARKINGPRICELISTSUB (ID INTEGER PRIMARY KEY AUTOINCREMENT, IDPARENT INTEGER, MINUTES INTEGER, PRICE DOUBLE)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS PARKING (ID INTEGER PRIMARY KEY AUTOINCREMENT, CARID INTEGER, IDPRICELIST INTEGER, STATUS INTEGER, STARTTIME DATETIME, ENDTIME DATETIME, PRICE DOUBLE)"),true);
			db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS COUNTRIESCACHE (LANGCOUNTRYCODE TEXT, DESCRIPTION TEXT)"),true);
			db_->BeginTransaction();
			pStmt_ = db_->CreateStatementN(L"INSERT INTO FUELTYPES (ID, CAPTION) VALUES (NULL, ?)");
			pStmt_->BindString(0, L"Gasoline");
			db_->ExecuteStatementN(*pStmt_);
			db_->CommitTransaction();
			if (pStmt_ != null)
			delete pStmt_;
			pStmt_ = null;
			db_->BeginTransaction();
			pStmt_ = db_->CreateStatementN(L"INSERT INTO FUELTYPES (ID, CAPTION) VALUES (NULL, ?)");
			pStmt_->BindString(0, L"Diesel");
			db_->ExecuteStatementN(*pStmt_);
			db_->CommitTransaction();
			if (pStmt_ != null)
			delete pStmt_;
			pStmt_ = null;
			this->isfiststart = true;
		} else {
			db_->Construct((L"/Home/data.db"), true);
			pEnum_ = db_->QueryN(L"SELECT DBVER, DISTANCEUNIT, VOLUMEUNIT, CONUNIT, CURRENCY FROM SETTINGS WHERE ID=1");
			if (!pEnum_) {
				this->isfiststart = true;
			} else {
				int dbver;
				pEnum_->Reset();
				pEnum_->MoveNext();
				pEnum_->GetIntAt(0, dbver);
				if (dbver < 2) {
					this->RecalculateAllCarConData();
					db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS COUNTRIESCACHE (LANGCOUNTRYCODE TEXT, DESCRIPTION TEXT)"),true);
					db_->ExecuteSql((L"ALTER TABLE SETTINGS ADD COLUMN AVGCALC INTEGER;"),true);
					if (pStmt_ != null)
					delete pStmt_;
					pStmt_ = null;
					pStmt_ = db_->CreateStatementN(L"UPDATE SETTINGS SET DBVER = ?, AVGCALC=1 WHERE ID=1");
					pStmt_->BindInt(0, 2);
					db_->ExecuteStatementN(*pStmt_);
					if (pStmt_ != null)
					delete pStmt_;
					pStmt_ = null;
				}
			}
			if (pEnum_ != null)
			delete pEnum_;
			pEnum_ = null;
		}
	}
	if (!this->GetCarData(lastselectedid, this->SelectedCar)) {
		this->SelectedCar.ID = 0;
		return false;
	} else {
		return true;
	}
}

void CarConClass::ClearCountryLangCodeCache() {
	if (db_ == null) {
		this->Create(0);
	}
	db_->ExecuteSql(L"DELETE FROM COUNTRIESCACHE", true);
}

void CarConClass::AddCountryLangCodeCache(Osp::Base::String langcountrycode, Osp::Base::String desc) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	pStmt_ = db_->CreateStatementN(L"INSERT INTO COUNTRIESCACHE (LANGCOUNTRYCODE, DESCRIPTION) VALUES (?, ?)");
	pStmt_->BindString(0, langcountrycode);
	pStmt_->BindString(1, desc);
	db_->ExecuteStatementN(*pStmt_);
	db_->CommitTransaction();
}

bool CarConClass::GetCountryLangCodeStart() {
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT LANGCOUNTRYCODE, DESCRIPTION FROM COUNTRIESCACHE");
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}

bool CarConClass::GetCountryLangCodeGetData(Osp::Base::String & langcountrycode, Osp::Base::String & desc) {
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetStringAt(0, langcountrycode);
		pEnum_->GetStringAt(1, desc);
		return true;
	} else {
		return false;
	}
}

bool CarConClass::GetCountryLangCodeEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}

void CarConClass::GetSettingsDataGetCaptions(int type, int typeval, String & retstr, String & retstrfull) {
	if (type == SETTINGSCAPTIONTYPE_VOLUME) {
		if (typeval == SETTINGSVOLUMEUNIT_GALON) {
			retstr = L"g";
			retstrfull = L"gallons";
		}
		if (typeval == SETTINGSVOLUMEUNIT_LITER) {
			retstr = L"L";
			retstrfull = L"liters";
		}
	}
	if (type == SETTINGSCAPTIONTYPE_DISTANCE) {
		if (typeval == SETTINGSDISTANCEUNIT_MILE) {
			retstr = L"mi";
			retstrfull = L"miles";
		}
		if (typeval == SETTINGSDISTANCEUNIT_KM) {
			retstr = L"km";
			retstrfull = L"kilometers";
		}
	}
	if (type == SETTINGSCAPTIONTYPE_CON) {
		if (typeval == SETTINGSCONTYPE_MPG)
			retstr = L"MPG";
		if (typeval == SETTINGSCONTYPE_GMI)
			retstr = L"g/100mi";
		if (typeval == SETTINGSCONTYPE_LKM)
			retstr = L"L/100km";
		if (typeval == SETTINGSCONTYPE_KML)
			retstr = L"km/L";
		if (typeval == SETTINGSCONTYPE_KMG)
			retstr = L"km/g";
		if (typeval == SETTINGSCONTYPE_MIL)
			retstr = L"mi/L";
		retstrfull = retstr;
	}
}

int CarConClass::GetSettingsDataGetDefaultCon(int volumeunit, int distanceunit) {
	if (volumeunit == SETTINGSVOLUMEUNIT_GALON) {
		if (distanceunit == SETTINGSDISTANCEUNIT_MILE) {
			return SETTINGSCONTYPE_MPG;
		} else {
			return SETTINGSCONTYPE_KMG;
		}
	} else {
		if (distanceunit == SETTINGSDISTANCEUNIT_MILE) {
			return SETTINGSCONTYPE_MIL;
		} else {
			return SETTINGSCONTYPE_LKM;
		}
	}
}

bool CarConClass::GetSettingsDataCheckConIfValid(int volumeunit, int distanceunit, int conunit) {
	if ((conunit == SETTINGSCONTYPE_MPG) || (conunit == SETTINGSCONTYPE_GMI)) {
		return ((volumeunit == SETTINGSVOLUMEUNIT_GALON) && (distanceunit == SETTINGSDISTANCEUNIT_MILE));
	}
	if ((conunit == SETTINGSCONTYPE_LKM) || (conunit == SETTINGSCONTYPE_KML)) {
		return ((volumeunit == SETTINGSVOLUMEUNIT_LITER) && (distanceunit == SETTINGSDISTANCEUNIT_KM));
	}
	if (conunit == SETTINGSCONTYPE_KMG) {
		return ((volumeunit == SETTINGSVOLUMEUNIT_GALON) && (distanceunit == SETTINGSDISTANCEUNIT_KM));
	}
	if (conunit == SETTINGSCONTYPE_MIL) {
		return ((volumeunit == SETTINGSVOLUMEUNIT_LITER) && (distanceunit == SETTINGSDISTANCEUNIT_MILE));
	}
	return false;
}

bool CarConClass::SaveSettingsData(SettingsData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	pEnum_ = db_->QueryN(L"SELECT DBVER, DISTANCEUNIT, VOLUMEUNIT, CONUNIT, CURRENCY FROM SETTINGS WHERE ID=1");
	if (!(!pEnum_)) {
		pStmt_ = db_->CreateStatementN(L"UPDATE SETTINGS SET DISTANCEUNIT = ?, VOLUMEUNIT = ?, CONUNIT = ?, CURRENCY = ?, AVGCALC = ? WHERE ID=1");
		pStmt_->BindInt(0, data.distanceunit);
		pStmt_->BindInt(1, data.volumeunit);
		pStmt_->BindInt(2, data.contype);
		pStmt_->BindString(3, data.currencycountrycode);
		pStmt_->BindInt(4, data.avgcalctype);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO SETTINGS (ID, DBVER, DISTANCEUNIT, VOLUMEUNIT, CONUNIT, CURRENCY, AVGCALC) VALUES (1, ?, ?, ?, ?, ?, ?)");
		pStmt_->BindInt(0, 2);
		pStmt_->BindInt(1, data.distanceunit);
		pStmt_->BindInt(2, data.volumeunit);
		pStmt_->BindInt(3, data.contype);
		pStmt_->BindString(4, data.currencycountrycode);
		pStmt_->BindInt(5, data.avgcalctype);
		db_->ExecuteStatementN(*pStmt_);
	}
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	db_->CommitTransaction();
	return true;
}

bool CarConClass::GetSettingsData(SettingsData & data) {
	data.distanceunit = SETTINGSDISTANCEUNIT_KM;
	data.volumeunit = SETTINGSVOLUMEUNIT_LITER;
	data.contype = SETTINGSCONTYPE_LKM;
	data.contype2 = 0;
	data.contype2str = L"";
	data.currencycountrycode = L"";
	data.avgcalctype = 0;
	int intvalue;
	String value;
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT DBVER, DISTANCEUNIT, VOLUMEUNIT, CONUNIT, CURRENCY, AVGCALC FROM SETTINGS WHERE ID=1");
	if (!(!pEnum_)) {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(1, intvalue);
		if ((intvalue == SETTINGSDISTANCEUNIT_KM) || (intvalue == SETTINGSDISTANCEUNIT_MILE)) {
			data.distanceunit = intvalue;
		}
		pEnum_->GetIntAt(2, intvalue);
		if ((intvalue == SETTINGSVOLUMEUNIT_LITER) || (intvalue == SETTINGSVOLUMEUNIT_GALON)) {
			data.volumeunit = intvalue;
		}
		pEnum_->GetIntAt(3, intvalue);
		if ((intvalue >= SETTINGSCONTYPE_MPG) && (intvalue <= SETTINGSCONTYPE_MIL)) {
			data.contype = intvalue;
		}
		if (GetSettingsDataCheckConIfValid(data.volumeunit, data.distanceunit, data.contype) == false) {
			data.contype = GetSettingsDataGetDefaultCon(data.volumeunit, data.distanceunit);
		}
		value = L"";
		if (pEnum_->GetColumnSize(4) > 0)
		pEnum_->GetStringAt(4, value);
		data.currencycountrycode = value;
		pEnum_->GetIntAt(5, intvalue);
		data.avgcalctype = intvalue;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	GetSettingsDataGetCaptions(SETTINGSCAPTIONTYPE_DISTANCE, data.distanceunit, data.distanceunitstr, data.distanceunitfullstr);
	GetSettingsDataGetCaptions(SETTINGSCAPTIONTYPE_VOLUME, data.volumeunit, data.volumeunitstr, data.volumeunitfullstr);
	GetSettingsDataGetCaptions(SETTINGSCAPTIONTYPE_CON, data.contype, data.contypestr, data.contypestr);
	if (data.contype == SETTINGSCONTYPE_MPG)
		data.contype2 = SETTINGSCONTYPE_GMI;
	if (data.contype == SETTINGSCONTYPE_LKM)
		data.contype2 = SETTINGSCONTYPE_KML;
	if (data.contype == SETTINGSCONTYPE_GMI)
		data.contype2 = SETTINGSCONTYPE_MPG;
	if (data.contype == SETTINGSCONTYPE_KML)
		data.contype2 = SETTINGSCONTYPE_LKM;
	GetSettingsDataGetCaptions(SETTINGSCAPTIONTYPE_CON, data.contype2, data.contype2str, data.contype2str);
	return true;
}
bool CarConClass::GetCarConGetLastData(int carid, int fueltype, CarConData & data, bool calccon) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	sql = L"SELECT STKM, PRICE, TIME, TYPE, CON, TRIP, FUELTYPE, FUELTYPES.CAPTION AS FUELTYPE_CAPTION FROM CON INNER JOIN FUELTYPES ON FUELTYPES.ID = CON.FUELTYPE WHERE CARID=";
	sql.Append(carid);
	sql.Append(L" ORDER BY TIME DESC, STKM DESC, CON.ID DESC LIMIT 2");
	data.CarID = SelectedCar.ID;
	data.StKm = 0;
	data.Price = 0;
	data.time = data.time.GetMinValue();
	data.Con = 0;
	data.PreviusCon = 0;
	data.AvgCon = 0;
	data.ConTolerance = 0.01;
	data.FuelType = 0;
	data.FuelTypeCaption = L"";
	data.Type = 0;
	DbEnumerator* pEnum2_;
	pEnum2_ = db_->QueryN(sql);
	if (!pEnum2_) {
		sql = L"SELECT STARTKM FROM CARS WHERE ID=";
		sql.Append(data.CarID);
		DbEnumerator* pEnumcd_;
		pEnumcd_ = db_->QueryN(sql);
		if(!(!pEnumcd_)) {
			pEnumcd_->Reset();
			pEnumcd_->MoveNext();
			pEnumcd_->GetIntAt(0, data.StKm);
		}
		delete pEnumcd_;
	} else {
		pEnum2_->Reset();
		pEnum2_->MoveNext();
		pEnum2_->GetIntAt(0, data.StKm);
		pEnum2_->GetDoubleAt(1, data.Price);
		this->DateFromSQLlite(*pEnum2_, 2, data.time);
		pEnum2_->GetIntAt(3, data.Type);
		pEnum2_->GetIntAt(5, data.Trip);
		pEnum2_->GetIntAt(6, data.FuelType);
		pEnum2_->GetStringAt(7, data.FuelTypeCaption);
		if (data.Type == 1) {
			pEnum2_->GetDoubleAt(4, data.Con);
		}
		if (calccon == true) {
			if (pEnum2_->MoveNext() == E_SUCCESS) {
				pEnum2_->GetDoubleAt(4, data.PreviusCon);
			}
		}
	}
	delete pEnum2_;
	if (fueltype > 0) {
		DbEnumerator* pEnum3_;
		sql = L"SELECT PRICE FROM CON WHERE FUELTYPE=";
		sql.Append(fueltype);
		sql.Append(L" ORDER BY TIME DESC, STKM DESC, ID DESC LIMIT 1");
		pEnum3_ = db_->QueryN(sql);
		if (!(!pEnum3_)) {
			pEnum3_->Reset();
			pEnum3_->MoveNext();
			pEnum3_->GetDoubleAt(0, data.Price);
		}
		delete pEnum3_;
	}
	if (calccon == true) {
		sql = L"SELECT AVG(CON) FROM CON WHERE TYPE=1 AND CARID=";
		sql.Append(carid);
		DbEnumerator* pEnumfcon_;
		pEnumfcon_ = db_->QueryN(sql);
		if (!(!pEnumfcon_)) {
			pEnumfcon_->Reset();
			pEnumfcon_->MoveNext();
			if (pEnumfcon_->GetColumnType(0) == DB_COLUMNTYPE_DOUBLE)
			pEnumfcon_->GetDoubleAt(0, data.AvgCon);
		}
		delete pEnumfcon_;
	}
	data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
	return true;
}
double CarConClass::GetCarConAvgSum(int carid) {
	double sumstl;
	int sumtrip;
	double avgsum = 0;
	String sql = L"SELECT SUM(STL), SUM(TRIP) FROM CON WHERE TYPE=1 AND CARID=";
	sql.Append(carid);
	DbEnumerator* pEnumfcon_;
	pEnumfcon_ = db_->QueryN(sql);
	if (!(!pEnumfcon_)) {
		pEnumfcon_->Reset();
		pEnumfcon_->MoveNext();
		if (pEnumfcon_->GetColumnType(0) == DB_COLUMNTYPE_DOUBLE)
		pEnumfcon_->GetDoubleAt(0, sumstl);
		if (pEnumfcon_->GetColumnType(1) == DB_COLUMNTYPE_INT)
		pEnumfcon_->GetIntAt(1, sumtrip);
		if (sumtrip > 0) {
			avgsum = (sumstl / sumtrip) * 100;
		}
	}
	delete pEnumfcon_;
	return avgsum;
}
bool CarConClass::GetCarConPrevNext(int itemid, int carid, Osp::Base::DateTime date, CarConData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	data.PreviusStKm = 0;
	data.Previustime = data.Previustime.GetMinValue();
	data.NextStKm = 0;
	data.NextType = 0;
	data.Nexttime = data.Nexttime.GetMaxValue();
	data.NextRecId = 0;
	data.NextCon = 0;
	data.NextStL = 0;
	data.NextTrip = 0;
	if (pStmt_ != null)
		delete pStmt_;
	pStmt_ = null;
	pStmt_ = db_->CreateStatementN(L"SELECT ID, CARID, TYPE, TIME, STKM FROM CON WHERE ((CARID=?) AND (TIME<=?) AND (ID!=?)) ORDER BY TIME DESC, STKM DESC, ID DESC LIMIT 1;");
	pStmt_->BindInt(0, carid);
	this->DateToSQLlite(*pStmt_,1,date);
	pStmt_->BindInt(2, itemid);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		data.PreviusStKm = 0;
		data.Previustime = data.Previustime.GetMinValue();
		sql = L"SELECT STARTKM FROM CARS WHERE ID=";
		sql.Append(carid);
		DbEnumerator* pEnumcd_;
		pEnumcd_ = db_->QueryN(sql);
		if(!(!pEnumcd_)) {
			pEnumcd_->Reset();
			pEnumcd_->MoveNext();
			pEnumcd_->GetIntAt(0, data.PreviusStKm);
		}
		delete pEnumcd_;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(4, data.PreviusStKm);
		pEnum_->GetIntAt(2, data.PreviusType);
		this->DateFromSQLlite(*pEnum_,3,data.Previustime);
	}
	if (pEnum_ != null)
		delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
		delete pStmt_;
	pStmt_ = null;
	pStmt_ = db_->CreateStatementN(L"SELECT ID, CARID, TYPE, TIME, STKM, STL, CON, TRIP FROM CON WHERE ((CARID=?) AND (STKM>=?) AND (TIME>=?) AND (ID!=?)) ORDER BY TIME DESC, STKM DESC, ID DESC LIMIT 1;");
	pStmt_->BindInt(0, carid);
	pStmt_->BindInt(1, data.PreviusStKm);
	this->DateToSQLlite(*pStmt_,2,date);
	pStmt_->BindInt(3, itemid);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		data.NextStKm = 0;
		data.NextType = 0;
		data.NextStL = 0;
		data.NextRecId = 0;
		data.NextCon = 0;
		data.NextTrip = 0;
		data.Nexttime = data.Nexttime.GetMaxValue();
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.NextRecId);
		pEnum_->GetIntAt(4, data.NextStKm);
		pEnum_->GetDoubleAt(5, data.NextStL);
		pEnum_->GetDoubleAt(6, data.NextCon);
		pEnum_->GetIntAt(7, data.NextTrip);
		pEnum_->GetIntAt(2, data.NextType);
		this->DateFromSQLlite(*pEnum_,3,data.Nexttime);
	}
	if (pStmt_ != null)
		delete pStmt_;
	pStmt_ = null;
	if (pEnum_ != null)
		delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetCarConData(int itemid, CarConData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT CON.ID, CARID, FUELTYPE, LOCATION, TYPE, TIME, STKM, STL, PRICE, REMARK, CON, TRIP, FUELTYPES.CAPTION AS FUELTYPECAPTION, LOCATIONS.CAPTION AS LOCATIONSCAPTION FROM CON LEFT JOIN FUELTYPES ON FUELTYPES.ID = CON.FUELTYPE LEFT JOIN LOCATIONS ON LOCATIONS.ID = CON.LOCATION WHERE CON.ID=";
	sql.Append(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		data.ID = 0;
		data.CarID = SelectedCar.ID;
		data.PreviusStKm = 0;
		data.Previustime = data.Previustime.GetMinValue();
		data.StL = 0;
		data.StKm = 0;
		data.NextStKm = 0;
		data.NextType = 0;
		data.Nexttime = data.Nexttime.GetMaxValue();
		data.NextRecId = 0;
		data.NextCon = 0;
		data.NextStL = 0;
		data.NextTrip = 0;
		sql = L"SELECT STARTKM FROM CARS WHERE ID=";
		sql.Append(data.CarID);
		DbEnumerator* pEnumcd_;
		pEnumcd_ = db_->QueryN(sql);
		if(!(!pEnumcd_)) {
			pEnumcd_->Reset();
			pEnumcd_->MoveNext();
			pEnumcd_->GetIntAt(0, data.PreviusStKm);
		}
		delete pEnumcd_;
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		pEnum_->GetIntAt(2, data.FuelType);
		pEnum_->GetIntAt(3, data.Location);
		pEnum_->GetIntAt(4, data.Type);
		this->DateFromSQLlite(*pEnum_,5,data.time);
		pEnum_->GetIntAt(6, data.StKm);
		pEnum_->GetDoubleAt(7, data.StL);
		pEnum_->GetDoubleAt(8, data.Price);
		if (pEnum_->GetColumnSize(9) > 0)
		pEnum_->GetStringAt(9, data.Remark);
		pEnum_->GetDoubleAt(10, data.Con);
		pEnum_->GetIntAt(11, data.Trip);
		if (pEnum_->GetColumnSize(12) > 0)
		pEnum_->GetStringAt(12, data.FuelTypeCaption);
		if (pEnum_->GetColumnSize(13) > 0)
		pEnum_->GetStringAt(13, data.LocationCaption);
		data.timejustdate.SetValue(data.time.GetYear(), data.time.GetMonth(), data.time.GetDay());
		if (pEnum_ != null)
			delete pEnum_;
		pEnum_ = null;
		if (pStmt_ != null)
			delete pStmt_;
		pStmt_ = null;
		pStmt_ = db_->CreateStatementN(L"SELECT ID, CARID, TYPE, TIME, STKM FROM CON WHERE ((CARID=?) AND (STKM<=?) AND (TIME<=?) AND (ID!=?)) ORDER BY TIME DESC, STKM DESC, ID DESC LIMIT 1;");
		pStmt_->BindInt(0, data.CarID);
		pStmt_->BindInt(1, data.StKm);
		this->DateToSQLlite(*pStmt_,2,data.timejustdate);
		pStmt_->BindInt(3, data.ID);
		pEnum_ = db_->ExecuteStatementN(*pStmt_);
		if(!pEnum_) {
			data.PreviusStKm = 0;
			data.Previustime = data.Previustime.GetMinValue();
			sql = L"SELECT STARTKM FROM CARS WHERE ID=";
			sql.Append(data.CarID);
			DbEnumerator* pEnumcd_;
			pEnumcd_ = db_->QueryN(sql);
			if(!(!pEnumcd_)) {
				pEnumcd_->Reset();
				pEnumcd_->MoveNext();
				pEnumcd_->GetIntAt(0, data.PreviusStKm);
			}
			delete pEnumcd_;
		} else {
			pEnum_->Reset();
			pEnum_->MoveNext();
			pEnum_->GetIntAt(4, data.PreviusStKm);
			pEnum_->GetIntAt(2, data.PreviusType);
			this->DateFromSQLlite(*pEnum_,3,data.Previustime);
		}
		if (pEnum_ != null)
			delete pEnum_;
		pEnum_ = null;
		if (pStmt_ != null)
			delete pStmt_;
		pStmt_ = null;
		pStmt_ = db_->CreateStatementN(L"SELECT ID, CARID, TYPE, TIME, STKM, STL, CON, TRIP FROM CON WHERE ((CARID=?) AND (STKM>=?) AND (TIME>=?) AND (ID!=?)) ORDER BY TIME DESC, STKM DESC, ID DESC LIMIT 1;");
		pStmt_->BindInt(0, data.CarID);
		pStmt_->BindInt(1, data.StKm);
		this->DateToSQLlite(*pStmt_,2,data.timejustdate);
		pStmt_->BindInt(3, data.ID);
		pEnum_ = db_->ExecuteStatementN(*pStmt_);
		if(!pEnum_) {
			data.NextStKm = 0;
			data.NextType = 0;
			data.NextStL = 0;
			data.NextRecId = 0;
			data.NextCon = 0;
			data.NextTrip = 0;
			data.Nexttime = data.Nexttime.GetMaxValue();
		} else {
			pEnum_->Reset();
			pEnum_->MoveNext();
			pEnum_->GetIntAt(0, data.NextRecId);
			pEnum_->GetIntAt(4, data.NextStKm);
			pEnum_->GetDoubleAt(5, data.NextStL);
			pEnum_->GetDoubleAt(6, data.NextCon);
			pEnum_->GetIntAt(7, data.NextTrip);
			pEnum_->GetIntAt(2, data.NextType);
			this->DateFromSQLlite(*pEnum_,3,data.Nexttime);
		}
		if (pStmt_ != null)
			delete pStmt_;
		pStmt_ = null;
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveCarConData(CarConData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE CON SET FUELTYPE = ?, LOCATION = ?, TYPE = ?, TIME = ?, STKM = ?, STL = ?, PRICE = ?, REMARK = ?, CON = ?, TRIP = ? WHERE ID=?");
		pStmt_->BindInt(0, data.FuelType);
		pStmt_->BindInt(1, data.Location);
		pStmt_->BindInt(2, data.Type);
		this->DateToSQLlite(*pStmt_, 3, data.time);
		pStmt_->BindInt(4, data.StKm);
		pStmt_->BindDouble(5, data.StL);
		pStmt_->BindDouble(6, data.Price);
		pStmt_->BindString(7, data.Remark);
		pStmt_->BindDouble(8, data.Con);
		pStmt_->BindInt(9, data.Trip);
		pStmt_->BindInt(10, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO CON (ID, CARID, FUELTYPE, LOCATION, TYPE, TIME, STKM, STL, PRICE, REMARK, CON, TRIP) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pStmt_->BindInt(0, data.CarID);
		pStmt_->BindInt(1, data.FuelType);
		pStmt_->BindInt(2, data.Location);
		pStmt_->BindInt(3, data.Type);
		this->DateToSQLlite(*pStmt_, 4, data.time);
		pStmt_->BindInt(5, data.StKm);
		pStmt_->BindDouble(6, data.StL);
		pStmt_->BindDouble(7, data.Price);
		pStmt_->BindString(8, data.Remark);
		pStmt_->BindDouble(9, data.Con);
		pStmt_->BindInt(10, data.Trip);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	if (data.NextRecId > 0) {
		//preracunaj vse recorde vkljucno od data.NextRecId, dokler ne prides do type=ok
		db_->BeginTransaction();
		if (data.NextRecId > 0) {
			pStmt_ = db_->CreateStatementN(L"UPDATE CON SET CON = ?, TRIP = ? WHERE ID=?");
			pStmt_->BindDouble(0, data.NextCon);
			pStmt_->BindInt(1, data.NextTrip);
			pStmt_->BindInt(2, data.NextRecId);
			db_->ExecuteStatementN(*pStmt_);
		}
		db_->CommitTransaction();
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
void CarConClass::RecalculateAllCarConData() {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;

	DbEnumerator * pEnumcar_ = db_->QueryN(L"SELECT ID, BRAND, MODEL FROM CARS ORDER BY ID");
	int carid;
	if (!(!pEnumcar_)) {
		pEnumcar_->Reset();
		while (pEnumcar_->MoveNext() == E_SUCCESS) {
			pEnumcar_->GetIntAt(0, carid);
			DbStatement* pStmtcon_ = db_->CreateStatementN(L"SELECT ID, CARID, TYPE, TIME, STKM, STL, CON FROM CON WHERE CARID=? ORDER BY TIME ASC, STKM ASC, CON.ID ASC");
			pStmtcon_->BindInt(0, carid);
			DbEnumerator * pEnumcon_ = db_->ExecuteStatementN(*pStmtcon_);
			if (!(!pEnumcon_)) {
				pEnumcon_->Reset();
				int priorrectype = 0;
				int thistype = 0;
				int recid = 0;
				while (pEnumcon_->MoveNext() == E_SUCCESS) {
					pEnumcon_->GetIntAt(0, recid);
					pEnumcon_->GetIntAt(2, thistype);
					if ((priorrectype == CARCONDATATYPEPARTIAL) || (priorrectype == CARCONDATATYPEMISSEDPARTIAL)) {
						if (thistype != CARCONDATATYPEAFTERPARTIAL) {
							thistype = CARCONDATATYPEAFTERPARTIAL;
							db_->BeginTransaction();
							pStmt_ = db_->CreateStatementN(L"UPDATE CON SET CON = ?, TYPE = ? WHERE ID=?");
							pStmt_->BindDouble(0, 0);
							pStmt_->BindInt(1, thistype);
							pStmt_->BindInt(2, recid);
							db_->ExecuteStatementN(*pStmt_);
							db_->CommitTransaction();
						}
					}
					priorrectype = thistype;
				}
			}
			if (pEnumcon_ != null)
			delete pEnumcon_;
			pEnumcon_ = null;
			if (pStmtcon_ != null)
			delete pStmtcon_;
			pStmtcon_ = null;
		}
	}
	if (pEnumcar_ != null)
	delete pEnumcar_;
	pEnumcar_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
}

bool CarConClass::DeleteCarConData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	String sql;
	bool candelete = true;
	if (candelete == true) {
		if (justcheck != true) {
			CarConData condata_;
			if (this->GetCarConData(itemid, condata_) == true) {
				if (condata_.NextRecId > 0) {
					condata_.NextTrip = condata_.NextStKm - condata_.PreviusStKm;
					if (condata_.NextType == 1) {
						condata_.NextCon = (condata_.NextStL / condata_.NextTrip) * 100;
					}
					this->SaveCarConData(condata_);
				}
				sql = L"DELETE FROM CON WHERE ID=";
				sql.Append(itemid);
				db_->ExecuteSql(sql, true);
			}
		}
		return true;
	} else {
		return false;
	}
	return false;
}
bool CarConClass::GetCarConDataListStart(int carid) {
	if (db_ == null) {
		this->Create(0);
	}
	pStmt_ = db_->CreateStatementN(L"SELECT CON.ID, CARID, FUELTYPE, LOCATION, TYPE, TIME, STKM, STL, PRICE, CON, FUELTYPES.CAPTION AS FUELTYPECAPTION, LOCATIONS.CAPTION AS LOCATIONSCAPTION, REMARK FROM CON LEFT JOIN FUELTYPES ON FUELTYPES.ID = CON.FUELTYPE LEFT JOIN LOCATIONS ON LOCATIONS.ID = CON.LOCATION WHERE CARID=? ORDER BY TIME DESC, STKM DESC, CON.ID DESC");
	pStmt_->BindInt(0, carid);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetCarConDataListStart(int carid, DateTime filterfromdate, DateTime filtertodate) {
	if (db_ == null) {
		this->Create(0);
	}
	pStmt_ = db_->CreateStatementN(L"SELECT CON.ID, CARID, FUELTYPE, LOCATION, TYPE, TIME, STKM, STL, PRICE, CON, FUELTYPES.CAPTION AS FUELTYPECAPTION, LOCATIONS.CAPTION AS LOCATIONSCAPTION, REMARK, TRIP FROM CON LEFT JOIN FUELTYPES ON FUELTYPES.ID = CON.FUELTYPE LEFT JOIN LOCATIONS ON LOCATIONS.ID = CON.LOCATION WHERE CARID=? AND TIME>=? AND TIME<=? ORDER BY TIME DESC, STKM DESC, CON.ID DESC");
	pStmt_->BindInt(0, carid);
	this->DateToSQLlite(*pStmt_, 1, filterfromdate);
	this->DateToSQLlite(*pStmt_, 2, filtertodate);
	pEnum_ = db_->ExecuteStatementN(*pStmt_);
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetCarConDataListGetData(CarConData & data) {
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		data.LocationCaption = L"";
		data.FuelTypeCaption = L"";
		data.Remark = L"";
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		pEnum_->GetIntAt(2, data.FuelType);
		pEnum_->GetIntAt(3, data.Location);
		pEnum_->GetIntAt(4, data.Type);
		this->DateFromSQLlite(*pEnum_,5,data.time);
		pEnum_->GetIntAt(6, data.StKm);
		pEnum_->GetDoubleAt(7, data.StL);
		pEnum_->GetDoubleAt(8, data.Price);
		pEnum_->GetDoubleAt(9, data.Con);
		if (pEnum_->GetColumnSize(10) > 0)
		pEnum_->GetStringAt(10, data.FuelTypeCaption);
		if (pEnum_->GetColumnSize(11) > 0)
		pEnum_->GetStringAt(11, data.LocationCaption);
		if (pEnum_->GetColumnSize(12) > 0)
		pEnum_->GetStringAt(12, data.Remark);
		pEnum_->GetIntAt(13, data.Trip);
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetCarConDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}

double CarConClass::ParseFloat(String & tmps) {
	double floatval;
	if (tmps.GetLength() < 1) {
	floatval = 0;
	} else {
	tmps.Replace(L"#",L"#");
	tmps.Replace(L"*",L"#");
	tmps.Replace(L".",L"#");
	tmps.Replace(L",",L"#");
	int decimalkapos=0;
	if (tmps.LastIndexOf(L"#",tmps.GetLength()-1,decimalkapos) == E_SUCCESS) {
		tmps.Replace(L"#",L".",decimalkapos);
		tmps.Replace(L"#","");
	}
	mchar tchar;
	String outputstr;
	for (int n=0; n<tmps.GetLength(); n++) {
		tmps.GetCharAt(n,tchar);
		if ((Character::IsDigit(tchar)) || (Character::ToString(tchar) == L".")) {
			outputstr += Character::ToString(tchar);
		}
	}
	tmps = outputstr;
	if (tmps.IndexOf(L".",0,decimalkapos) == E_SUCCESS) {
		if (decimalkapos == tmps.GetLength()-1) {
			tmps.Replace(L".","");
		}
	}
	if (Double::Parse(tmps, floatval) == E_SUCCESS) {
		tmps.Format(25,L"%4.2f", floatval);
	} else {
		floatval = 0;
		tmps = L"0";
	}
	}
	return floatval;
}

int CarConClass::ParseInteger(String & tmps) {
	int intval;
	tmps = Double::ToString(ParseFloat(tmps));
	int decimalkapos=0;
	if (tmps.IndexOf(L".",0,decimalkapos) == E_SUCCESS) {
	tmps.SetLength(decimalkapos);
	}
	if (Integer::Parse(tmps, intval) != E_SUCCESS) {
		intval = 0;
		tmps = L"0";
	}
	return intval;
}

CarConClass::~CarConClass() {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pEnum_ = null;
	if (db_ != null)
	delete db_;
}

bool CarConClass::GetTripDataListStart(int carid) {
	if (db_ == null) {
		this->Create(0);
	}
	pEnum_ = db_->QueryN(L"SELECT TRIP.ID, CARID, STARTTIME, STARTLOCATION, STARTODOMETER, ENDTIME, ENDLOCATION, ENDODOMETER, CALCEXPENSES, REALEXPENSES, REMARK, LOCATIONS.CAPTION AS STARTLOCATIONCAPTION, LOCATION2.CAPTION AS ENDLOCATIONCAPTION FROM TRIP LEFT JOIN LOCATIONS ON LOCATIONS.ID = STARTLOCATION LEFT JOIN LOCATIONS AS LOCATION2 ON LOCATION2.ID = ENDLOCATION WHERE CARID=" + Osp::Base::Integer::ToString(carid) + L" ORDER BY TRIP.ID DESC");
	if(!pEnum_) {
		return false;
	} else {
		pEnum_->Reset();
		return true;
	}
}
bool CarConClass::GetTripDataListGetData(TripData & data) {
	if (pEnum_->MoveNext() ==  E_SUCCESS) {
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		this->DateFromSQLlite(*pEnum_,2,data.StartTime);
		pEnum_->GetIntAt(3, data.StartLocation);
		pEnum_->GetIntAt(4, data.StartOdometer);
		this->DateFromSQLlite(*pEnum_,5,data.EndTime);
		pEnum_->GetIntAt(6, data.EndLocation);
		pEnum_->GetIntAt(7, data.EndOdometer);
		pEnum_->GetDoubleAt(8, data.CalcExpenses);
		pEnum_->GetDoubleAt(9, data.RealExpenses);
		if (pEnum_->GetColumnSize(10) > 0) {
			pEnum_->GetStringAt(10, data.Remark);
		} else {
			data.Remark = L"";
		}
		if (pEnum_->GetColumnSize(11) > 0) {
			pEnum_->GetStringAt(11, data.StartLocationCaption);
		} else {
			data.StartLocationCaption = L"";
		}
		if (pEnum_->GetColumnSize(12) > 0) {
			pEnum_->GetStringAt(12, data.EndLocationCaption);
		} else {
			data.EndLocationCaption = L"";
		}
		return true;
	} else {
		return false;
	}
}
bool CarConClass::GetTripDataListEnd(void) {
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return true;
}
bool CarConClass::GetTripData(int itemid, TripData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	sql = L"SELECT TRIP.ID, CARID, STARTTIME, STARTLOCATION, STARTODOMETER, ENDTIME, ENDLOCATION, ENDODOMETER, CALCEXPENSES, REALEXPENSES, REMARK, LOCATIONS.CAPTION AS STARTLOCATIONCAPTION, LOCATION2.CAPTION AS ENDLOCATIONCAPTION, PRICEPERL, STOSEB FROM TRIP LEFT JOIN LOCATIONS ON LOCATIONS.ID = STARTLOCATION LEFT JOIN LOCATIONS AS LOCATION2 ON LOCATION2.ID = ENDLOCATION WHERE TRIP.ID=" + Osp::Base::Integer::ToString(itemid);
	pEnum_ = db_->QueryN(sql);
	if(!pEnum_) {
		returnvalue = false;
	} else {
		pEnum_->Reset();
		pEnum_->MoveNext();
		pEnum_->GetIntAt(0, data.ID);
		pEnum_->GetIntAt(1, data.CarID);
		this->DateFromSQLlite(*pEnum_,2,data.StartTime);
		pEnum_->GetIntAt(3, data.StartLocation);
		pEnum_->GetIntAt(4, data.StartOdometer);
		this->DateFromSQLlite(*pEnum_,5,data.EndTime);
		pEnum_->GetIntAt(6, data.EndLocation);
		pEnum_->GetIntAt(7, data.EndOdometer);
		pEnum_->GetDoubleAt(8, data.CalcExpenses);
		pEnum_->GetDoubleAt(9, data.RealExpenses);
		if (pEnum_->GetColumnSize(10) > 0) {
			pEnum_->GetStringAt(10, data.Remark);
		} else {
			data.Remark = L"";
		}
		if (pEnum_->GetColumnSize(11) > 0) {
			pEnum_->GetStringAt(11, data.StartLocationCaption);
		} else {
			data.StartLocationCaption = L"";
		}
		if (pEnum_->GetColumnSize(12) > 0) {
			pEnum_->GetStringAt(12, data.EndLocationCaption);
		} else {
			data.EndLocationCaption = L"";
		}
		pEnum_->GetDoubleAt(13, data.PricePerL);
		pEnum_->GetIntAt(14, data.StOseb);
		returnvalue = true;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
bool CarConClass::SaveTripData(TripData & data) {
	if (db_ == null) {
		this->Create(0);
	}
	db_->BeginTransaction();
	if (data.ID > 0) {
		pStmt_ = db_->CreateStatementN(L"UPDATE TRIP SET CARID = ?, STARTTIME = ?, STARTLOCATION = ?, STARTODOMETER = ?, ENDTIME = ?, ENDLOCATION = ?, ENDODOMETER = ?, CALCEXPENSES = ?, REALEXPENSES = ?, REMARK = ?, PRICEPERL = ?, STOSEB = ? WHERE TRIP.ID=?");
		pStmt_->BindInt(0, data.CarID);
		this->DateToSQLlite(*pStmt_,1,data.StartTime);
		pStmt_->BindInt(2, data.StartLocation);
		pStmt_->BindInt(3, data.StartOdometer);
		this->DateToSQLlite(*pStmt_,4,data.EndTime);
		pStmt_->BindInt(5, data.EndLocation);
		pStmt_->BindInt(6, data.EndOdometer);
		pStmt_->BindDouble(7, data.CalcExpenses);
		pStmt_->BindDouble(8, data.RealExpenses);
		pStmt_->BindString(9, data.Remark);
		pStmt_->BindDouble(10, data.PricePerL);
		pStmt_->BindInt(11, data.StOseb);
		pStmt_->BindInt(12, data.ID);
		db_->ExecuteStatementN(*pStmt_);
	} else {
		pStmt_ = db_->CreateStatementN(L"INSERT INTO TRIP (ID, CARID, STARTTIME, STARTLOCATION, STARTODOMETER, ENDTIME, ENDLOCATION, ENDODOMETER, CALCEXPENSES, REALEXPENSES, REMARK, PRICEPERL, STOSEB) VALUES (NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
		pStmt_->BindInt(0, data.CarID);
		this->DateToSQLlite(*pStmt_,1,data.StartTime);
		pStmt_->BindInt(2, data.StartLocation);
		pStmt_->BindInt(3, data.StartOdometer);
		this->DateToSQLlite(*pStmt_,4,data.EndTime);
		pStmt_->BindInt(5, data.EndLocation);
		pStmt_->BindInt(6, data.EndOdometer);
		pStmt_->BindDouble(7, data.CalcExpenses);
		pStmt_->BindDouble(8, data.RealExpenses);
		pStmt_->BindString(9, data.Remark);
		pStmt_->BindDouble(10, data.PricePerL);
		pStmt_->BindInt(11, data.StOseb);
		db_->ExecuteStatementN(*pStmt_);
	}
	db_->CommitTransaction();
	if (data.ID <= 0) {
		pEnum_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnum_)) {
			pEnum_->Reset();
			pEnum_->MoveNext();
			int intvalue;
			pEnum_->GetIntAt(0, intvalue);
			data.ID = intvalue;
		}
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	if (pStmt_ != null)
	delete pStmt_;
	pStmt_ = null;
	return true;
}
bool CarConClass::DeleteTripData(int itemid, bool justcheck) {
	if (db_ == null) {
		this->Create(0);
	}
	bool returnvalue;
	String sql;
	bool candelete = true;
	if (candelete == true) {
		if (justcheck != true) {
			sql = L"DELETE FROM TRIP WHERE ID=";
			sql.Append(itemid);
			db_->ExecuteSql(sql, true);
		}
		returnvalue = true;
	} else {
		returnvalue = false;
	}
	if (pEnum_ != null)
	delete pEnum_;
	pEnum_ = null;
	return returnvalue;
}
