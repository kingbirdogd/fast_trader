class Trade extends React.Component {
  constructor(props) {
    super(props)
    this.state = {}
  }
  
  componentDidMount() {

  }
  
  render() {
    
    var tb1 = []
    for(var i=0; i < 20; i++) {
      tb1.push(
      <tr key={'tb1-'+i}>
        <td> <input type="text" placeholder="輸入正股" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" value="購" disabled readOnly /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <button className="btn btn-sm btn-secondary" type="button">開啟</button> </td>
        <td> <button className="btn btn-sm btn-secondary" type="button">即買</button> </td>
        <td> <button className="btn btn-sm btn-secondary" type="button">開啟</button> </td>
        <td> </td>
        <td> <input type="text" value="0" disabled readOnly /> </td>
        <td> <input type="text" value="0" disabled readOnly /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <input type="text" /> </td>
        <td> <button className="btn btn-sm btn-secondary" type="button">開啟</button> </td>
        <td> <button className="btn btn-sm btn-secondary" type="button">即賣</button> </td>
        <td> </td>
      </tr>)
    }
    
    return(

<div className="row">
<div className="col-12 col-sm-12">

<div className="x-scroll">
<table className="table table-striped table-trade">
<thead>
  <tr>
    <th scope="col" width="55">股</th>
    <th scope="col" width="75">證</th>
    <th scope="col" width="35">類別</th>
    <th scope="col" width="75">量</th>
    <th scope="col" width="75">觸發買價<br />(股)</th>
    <th scope="col" width="105">限買價<br />(證)</th>
    <th scope="col" width="75">早買量</th>
    <th scope="col" width="75">比例買</th>
    <th scope="col" width="75">觸發賣價<br />(股)</th>
    <th scope="col" width="45">限賣價<br />(證)</th>
    <th scope="col" width="45">早賣量</th>
    <th scope="col" width="60">比例賣</th>
    <th scope="col" width="50">監控<br />(買)</th>
    <th scope="col" width="45">即買</th>
    <th scope="col" width="60">自動監控<br />賣出</th>
    <th scope="col" width="35">狀態</th>
    <th scope="col" width="50">被觸發<br />股價</th>
    <th scope="col" width="50">被觸發<br />證價</th>
    <th scope="col" width="70">成交量<br />(證)</th>
    <th scope="col" width="70">止損價</th>
    <th scope="col" width="70">止盈價</th>
    <th scope="col" width="50">監控<br />(賣)</th>
    <th scope="col" width="45">即賣</th>
    <th scope="col" width="35">狀態</th>
  </tr>
</thead>
<tbody>
{tb1}
</tbody>
</table>
</div>

</div>
</div>

    )
  }
}
